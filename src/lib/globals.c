#include "globals.h"

// Initializing the global structure with default values
struct globals globals = {
    .config = CONFIG_INIT,
    // This is null as per documentation of uthash
    .hashl = NULL,
    // Current sequence recieved is 0
    .current_seq = 0,
    // Used by nodeB for detecting if dummy packet arrived or not
    .last_bit_arrived = false,
    .last_bit_send = false,
    .total_retrans = 0
};

/**
 * Calculates difference between two timeval.
 */
unsigned int time_diff_micro(struct timeval end, struct timeval start){
  return ((end.tv_sec - start.tv_sec) * 1000000) + (end.tv_usec - start.tv_usec);
}

unsigned int to_micro(struct timeval tv){
  return tv.tv_sec* 1000000 + tv.tv_usec;
}

int send_nack_packet()
{
    vlong total_len = 0;
    My402ListElem *elem=NULL;
    elem=My402ListFirst(&globals.nackl);
    for (elem=My402ListFirst(&globals.nackl);
         elem != NULL && ( ((struct node*)(elem->obj))->seq_num < globals.current_seq || globals.last_bit_arrived);
         elem=My402ListNext(&globals.nackl, elem)) {
        struct node *data_node = (elem->obj);

        char *buffer;
        vlong payload_size = create_nack_packet(&buffer, data_node->seq_num);

        int packet_size = payload_size + C_HLEN;
        char *packet = malloc(packet_size);

        create_packet(packet, 1, 2, 1, DATA_PORT, buffer, payload_size);

        send_packet_on_line(INF0, packet, packet_size);

        free(buffer);
        total_len++;
    }
    printf("[SIGNAL] NACK LEN: %llu\n", total_len);
    return 0;
}
