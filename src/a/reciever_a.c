#include "reciever_a.h"
#include "filter.h"

bool is_nack_list_empty() {
    if ((globals.nackl).num_members == 0) {
        return true;
    } else {
        return false;
    }
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
    int payload_size = data_size - C_HLEN;

    printf("DEBUG] Recived a NACK\n");
    int packet_type = get_recieved_packet_type(payload);

    switch (packet_type) {
        case NACK_PACKET:
            nack_packet_handler(payload, payload_size);
            break;
        default:
            printf("[SUMMARY] Unknown packet type\n");
    }

    //memset(payload_size, '\0', PACKET_LEN);
}

void *reciever(void *v)
{
    int saddr_size , data_size;
    struct sockaddr saddr;
    unsigned char *buffer = (unsigned char *) malloc(PACKET_LEN);

    printf("Starting...\n");

    int sock_raw = socket( AF_PACKET , SOCK_RAW , htons(ETH_P_ALL)) ;

    if (sock_raw < 0) {
        perror("Socket Error");
        return;
    }

    while (1){
        memset(buffer, '\0', PACKET_LEN);
        sniff_packet(sock_raw, buffer, saddr);
    }
}

void nack_packet_handler(char *buffer, int size_recieved){
    printf("[DEBUG] NACK recieved\n");
    char *checksum, *payload;
    vlong payload_size = get_packet_data_nack(buffer, size_recieved, &checksum, &payload);

    vlong seq_num = atoi(payload);

    hashed_link *hash_node = (hashed_link *)(find_hashl(seq_num));
    if (hash_node == NULL) {
        DBG("Hash node is null, this is not possible");
        exit(1);
    }
    struct node *data_node = (struct node*)((hash_node->data_node_ptr)->obj);

    // Retransmit the data back again with sequence number
    globals.total_retrans++;
    printf("[RETRANS SEND]: TOTAL %llu", globals.total_retrans);
    int n = send_packet(data_node);
    if (n < 0) {
        perror("Retransmiston: Error in sending packet");
        exit(1);
    }

    free(payload);
    free(checksum);
}
