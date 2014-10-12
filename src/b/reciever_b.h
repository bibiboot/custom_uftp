#include "globals.h"

void *reciever(void *val);

int recv_packet(char *buffer, int payload_size);

void data_packet_handler(char *buffer, int size_recieved);

void dummy_packet_handler(char *buffer, int size_recieved);
