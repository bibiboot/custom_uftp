#include "globals.h"
#include <linux/if_ether.h>

uint64_t get_pattern(unsigned char *packet);

void set_pattern(unsigned char *packet, uint64_t pttn_num);

void create_log_file();

int set_promisc(char *interface, int sock);

bool is_nack_list_empty();

bool is_last_packet_recieved();
