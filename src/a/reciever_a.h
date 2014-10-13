#include "globals.h"

void *reciever(void *v);

void sniff_packet(int sock_raw, char *buffer, struct sockaddr saddr);

void data_packet_handler(char *buffer, int size_recieved);

void dummy_packet_handler(char *buffer, int size_recieved);

void nack_packet_handler(char *buffer, int size_recieved);
