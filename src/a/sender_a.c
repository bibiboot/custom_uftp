#include "sender_a.h"

void* sender(void *v){
    // Iterate the data list and send data
    My402ListElem *elem=NULL;
    bool is_retransmitted = false;
    vlong num_packets = 0;

    printf("[SUMMARY] Start Sending.....\n");
    for (elem=My402ListFirst(&globals.datal); elem != NULL;
            elem=My402ListNext(&globals.datal, elem)) {
        struct node *data_node = (elem->obj);
        int n = send_packet(data_node, is_retransmitted);
        num_packets++;
        if (n < 0) {
            perror("Error on send");
            exit(1);
        }
    }
    printf("[SUMMARY] Sender has tried sending complete file.....\n");

    // The nodeA knows that last bit is send
    globals.last_bit_send = true;

    // Send dummy data denotes the end of sending data
    send_dummy_packet();
    printf(KGRN "[SUMMARY] Start time : %llu ms, Packets : %llu, Retransmission : %llu, Nack recv : %llu \n" RESET,
            to_milli(globals.a_sender_start), num_packets, globals.total_retrans,globals.total_nack_recv);
}

int send_packet(struct node *data_node, bool is_retransmitted)
{
    // Create data packet
    // Add checksum and packet_type bit
    char *payload;
    vlong payload_size = create_data_packet(data_node->mem_ptr, data_node->size,
                                            data_node->seq_num, &payload,
                                            is_retransmitted);

    int packet_size = payload_size + C_HLEN;
    char *packet = malloc(packet_size);

    create_packet(packet, ROUTER_MAC, globals.own_node,
                  globals.other_node, DATA_PORT, payload, payload_size);

    send_packet_on_line(INF0, packet, packet_size);

    free(payload);
    return packet_size;
}

int send_dummy_packet(){
    // Create data packet
    // Add checksum and packet_type bit
    char *payload;
    vlong payload_size = create_dummy_packet(&payload);
    int packet_size = payload_size + C_HLEN;
    char *packet = malloc(packet_size);
    create_packet(packet, ROUTER_MAC, globals.own_node,
                  globals.other_node, DATA_PORT, payload,
                  payload_size);

    int i;
    for (i = 0; i <= NUM_DUMMY_PACKETS; i++)
        send_packet_on_line(INF0, packet, packet_size);

    free(payload);
    return 0;
}
