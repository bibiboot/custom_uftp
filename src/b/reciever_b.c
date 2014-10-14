#include "reciever_b.h"
#include "filter.h"
#include "print_packet.h"
#include "util.h"

void sniff_packet(int sock_raw, char *buffer, struct sockaddr saddr);

void *reciever(void *val)
{
    int saddr_size , data_size;
    struct sockaddr saddr;
    unsigned char *buffer = (unsigned char *) malloc(PACKET_LEN);
    memset(buffer, '\0', PACKET_LEN);

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
            printf("[DEBUG] NACK list is empty and last bit received\n");
            goto COMPLETE_FILE_REACHED;
        }

        sniff_packet(sock_raw, buffer, saddr);
        memset(buffer, '\0', PACKET_LEN);
    }
COMPLETE_FILE_REACHED:
    gettimeofday(&globals.b_reciever_end, NULL);
    printf(KGRN "[SUMMARY] File received, duplicates : %llu, end-time : %llu ms\n" RESET,
            globals.total_retrans,
            to_milli(globals.b_reciever_end));
    write_data_list_to_file(globals.recv_filename);
    // Cancel the other thread
    printf("[DEBUG] Cancel the timer thread\n");
    fflush(stdout);
    pthread_cancel(globals.sender_th);
}

void sniff_packet(int sock_raw, char *buffer, struct sockaddr saddr)
{
    int saddr_size = sizeof saddr;
    int data_size = recvfrom(sock_raw , buffer , PACKET_LEN ,
                             0 , &saddr , (socklen_t*)&saddr_size);
    if(data_size <0 ) {
        printf("Error: Recvfrom error , failed to get packets\n");
        return ;
    }

    if ( !is_data_allowed(buffer) )
        return;

    unsigned char *payload = buffer + C_HLEN;
    int payload_size = (data_size - C_HLEN);

    //print_human_read_payload(buffer, data_size);
    recv_packet(payload, payload_size);
}

int recv_packet(char *buffer, int payload_size){
    // Check the packet with checksum
    // If no match then return i.e. drop packet
    int packet_type = get_recieved_packet_type(buffer);

    switch (packet_type) {
        case DATA_PACKET:
            data_packet_handler(buffer, payload_size);
            //printf(KGRN "Data receieved\n" RESET);
            break;
        case DUMMY_PACKET:
            dummy_packet_handler(buffer, payload_size);
            //printf(KGRN "Dummy receieved\n" RESET);
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
        //printf(KRED "[DEBUG] Duplicate seq num : %llu\n" RESET, seq_num_int);
        globals.total_retrans++;
        free(checksum);
        return;
    }

    // Update the maximum sequence read
    if (seq_num_int > globals.current_seq) {
        globals.current_seq = seq_num_int;
    }

    // Checksum matched and sequence number known
    // Update the memory pointer
    update_mem_ptr_data_link(payload, sq_num, payload_size);

    // Remove the node from the nack list
    delete_node_nack_list(sq_num);

    free(checksum);
}

void dummy_packet_handler(char *buffer, int size_recieved) {

    if (globals.last_bit_arrived) {
        printf("KRED [DEBUG] Duplicate Dummy packet received\n" RESET);
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
