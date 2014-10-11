#include "reciever_a.h"

void *reciever(void *v){
    char buffer[globals.config.read_buffer_size];
    bzero(buffer,globals.config.read_buffer_size);

    struct sockaddr_in from;
    int fromlen = sizeof(struct sockaddr_in);

    printf("[DEBUG] Waiting for NACK.....\n");
    while (1){
        int size_recieved=recvfrom(globals.a_recv_fd,
                                   buffer, globals.config.read_buffer_size, 0,
                       (struct sockaddr *)&from, &fromlen);
        if (size_recieved < 0) {
            perror("Error in recv");
            exit(1);
        }
        printf("DEBUG] Recived a NACK\n");
        int packet_type = get_recieved_packet_type(buffer);

        switch (packet_type) {
            case NACK_PACKET:
                nack_packet_handler(buffer, size_recieved);
                break;
            default:
                printf("[SUMMARY] Unknown packet type\n");
        }
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
