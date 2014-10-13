#include "filter.h"

#include<netinet/in.h>
#include<errno.h>
#include<netdb.h>
#include<stdio.h> //For standard things
#include<stdlib.h>    //malloc
#include<string.h>    //strlen

#include<netinet/ip_icmp.h>   //Provides declarations for icmp header
#include<netinet/udp.h>   //Provides declarations for udp header
#include<netinet/tcp.h>   //Provides declarations for tcp header
#include<netinet/ip.h>    //Provides declarations for ip header
#include<netinet/if_ether.h>  //For ETH_P_ALL
#include<net/ethernet.h>  //For ether_header
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/ioctl.h>
#include<sys/time.h>
#include<sys/types.h>
#include<unistd.h>

bool is_allowed(unsigned char *packet)
{
    struct custom_ethernet *eth_header = (struct custom_ethernet*)packet;
    if (ntohs(eth_header->dest_mac) == globals.own_node)
        return true;
    return false;
}

bool is_nack_allowed(unsigned char *packet)
{
    struct custom_ethernet *eth_header = (struct custom_ethernet*)packet;
    struct custom_udp *udp_header = (struct custom_udp*)(packet + C_ETHLEN + C_IPLEN);
    if (ntohs(eth_header->dest_mac) == globals.own_node && ntohs(udp_header->port) == NACK_PORT)
        return true;
    return false;
}

bool is_data_allowed(unsigned char *packet)
{
    struct custom_ethernet *eth_header = (struct custom_ethernet*)packet;
    struct custom_udp *udp_header = (struct custom_udp*)(packet + C_ETHLEN + C_IPLEN);
    if (ntohs(eth_header->dest_mac) == globals.own_node && ntohs(udp_header->port) == DATA_PORT)
        return true;
    return false;
}
