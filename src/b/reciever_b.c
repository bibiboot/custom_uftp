#include "reciever_b.h"
#include "filter.h"
#include "print_packet.h"

#define PACKET_LEN 65536
void sniff_packet(int sock_raw, char *buffer, struct sockaddr saddr);

bool is_nack_list_empty() {
    if ((globals.nackl).num_members == 0) {
        return true;
    } else {
        return false;
    }
}

bool is_last_packet_recieved() {
    return globals.last_bit_arrived;
}

void *reciever(void *val)
{
    int saddr_size , data_size;
    struct sockaddr saddr;
    unsigned char *buffer = (unsigned char *) malloc(PACKET_LEN);
    memset(buffer, '\0', PACKET_LEN);

    printf("Starting...\n");

    int sock_raw = socket( AF_PACKET , SOCK_RAW , htons(ETH_P_ALL)) ;

    if (sock_raw < 0) {
        perror("Socket Error");
        return;
    }

    /**
     * Set it on promiscous mode,
     * Otherwise it won't sniff packet
     * Which do not belong to him
     */
    set_promisc(INF0, sock_raw);

    printf("[DEBUG] Waiting for data to arrive......\n");
    while (1){
        // If last packet is recieved
        // break out of the loop
        if (is_last_packet_recieved() &&
            is_nack_list_empty()) {
            // Delete the nack timer
            // Break out of the loop
            // Return
            // TODO: Delete the nack timer
            printf("[DEBUG] NACK list is empty and last bit received\n");
            goto COMPLETE_FILE_REACHED;
        }

        sniff_packet(sock_raw, buffer, saddr);
        memset(buffer, '\0', PACKET_LEN);
    }
COMPLETE_FILE_REACHED:
    gettimeofday(&globals.b_reciever_end, NULL);
    printf("[SUMMARY] Complete file received, end-time : %u\n", to_micro(globals.b_reciever_end));
    write_data_list_to_file(globals.recv_filename);
    // Cancel the other thread
    printf("[DEBUG] Cancel the timer thread\n");
    pthread_cancel(globals.sender_th);
}

void sniff_packet(int sock_raw, char *buffer, struct sockaddr saddr)
{
    int saddr_size = sizeof saddr;
    // Receive a packet
    int data_size = recvfrom(sock_raw , buffer , PACKET_LEN ,
                             0 , &saddr , (socklen_t*)&saddr_size);
    if(data_size <0 )
    {
        printf("Error: Recvfrom error , failed to get packets\n");
        return ;
    }

    if ( !is_allowed(buffer) )
        return;


    unsigned char *payload = buffer + C_HLEN;
    int payload_size = (data_size - 8);

    printf("Header size = %d\n", C_HLEN);
    printf("Data size xxx = %d\n", data_size);
    printf("Payload size = %d\n", payload_size);
    printf("Payload size = %d\n", data_size - 8);

    print_human_read_payload(buffer, data_size);
    //payload_size = 50;
    recv_packet(payload, payload_size);

    memset(buffer, '\0', PACKET_LEN);
}

int recv_packet(char *buffer, int payload_size){
    // Check the packet with checksum
    // If no match then return i.e. drop packet
    int packet_type = get_recieved_packet_type(buffer);

    switch (packet_type) {
        case DATA_PACKET:
            data_packet_handler(buffer, payload_size);
            printf(KGRN "Data receieved\n" RESET);
            break;
        case DUMMY_PACKET:
            dummy_packet_handler(buffer, payload_size);
            printf(KGRN "Dummy receieved\n" RESET);
            break;
        default:
            printf(KRED "[DEBUG] packet type not matched, packet dropped\n" RESET);
    }
    return 0;
}

void data_packet_handler(char *buffer, int size_recieved) {

    char *seq_num, *checksum, *payload;
    vlong payload_size = get_packet_data(buffer, size_recieved, &seq_num, &checksum, &payload);
    vlong sq_num= atoll(seq_num);

    vlong seq_num_int = atoi(seq_num);

    // Check if duplicate packet
    if (is_duplicate(seq_num_int)) {
        DBG("[DUPLICATE RECV] SEQ NUM: %llu", seq_num_int);
        // Free all the memory taken
        return;
    }

    // Update the maximum sequence read
    if (seq_num_int > globals.current_seq) {
        globals.current_seq = seq_num_int;
    }

    //DBG("[DATA RECV] SIZE RECV: %d, SEQ: %s,  CURR MAX: %llu",
     //   size_recieved, seq_num, globals.current_seq);

    // Checksum matched and sequence number known
    // Update the memory pointer
    update_mem_ptr_data_link(payload, sq_num, payload_size);

    // Remove the node from the nack list
    delete_node_nack_list(sq_num);

    //TODO: Free the seq_num and checksum
    free(checksum);
}

void dummy_packet_handler(char *buffer, int size_recieved) {

    if (globals.last_bit_arrived) {
        printf("[DEBUG] Duplicate Dummy packet received\n");
        return;
    }

    gettimeofday(&globals.dummy_reached, NULL);
    // On the bit for last bit arrived
    globals.last_bit_arrived = true;
    printf("[DEBUG] Dummy packet receieved, denoting the end of the last bit : [%s]\n", buffer);

    // Get checksum and filename of the destination
    char *checksum, *payload;
    vlong payload_size = get_packet_data_dummy(buffer, size_recieved, &checksum, &payload);
    // Payload here is the filename
    free(checksum);
}
