#include "globals.h"
#include "socket_util.h"
#include "socket_addr_create.h"
#include "packet_send.h"

void create_packet(void *packet, uint16_t dest_mac, uint16_t src_ip, uint16_t dest_ip,
                   uint16_t port, char *payload, int payload_size)
{
    /*userdata in ethernet frame*/
    int header_size = sizeof(struct custom_ethernet) + sizeof(struct custom_ip) + sizeof(struct custom_udp);
    unsigned char* data = packet + C_HLEN;

    struct custom_ethernet *eth_header = (struct custom_ethernet*)packet;
    struct custom_ip *ip_header = (struct custom_ip*)(packet + C_ETHLEN);
    struct custom_udp *udp_header = (struct custom_udp*)(packet + C_ETHLEN + C_IPLEN);

    eth_header->dest_mac = htons(dest_mac);
    ip_header->src_ip = htons(src_ip);
    ip_header->dest_ip = htons(dest_ip);
    udp_header->port = htons(port);

    memcpy((void*)(packet+header_size), payload, payload_size+1);
}
