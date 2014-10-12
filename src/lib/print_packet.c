#include "print_packet.h"

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

void print_human_read_payload(unsigned char *packet, int packet_size)
{
    printf(KCYN "SNIFF: " RESET);
    struct custom_ethernet *eth_header = (struct custom_ethernet*)packet;
    struct custom_ip *ip_header = (struct custom_ip*)( packet + C_ETHLEN );
    struct custom_udp *udp_header = (struct custom_udp*)( packet + C_ETHLEN + C_IPLEN );
    unsigned char *payload = packet + C_HLEN;

    printf("Total [%d]: Header size [%d]: Payload [%d]: %s\n",
            packet_size, C_HLEN,packet_size - C_HLEN, payload);
    printf("++++++++++++++++++++++++++++++++++++++++++++\n");
    printf("| %s ",
            print_human_format_number(ntohs(eth_header->dest_mac), "ETHERNET"));
    printf("| %s ",
            print_human_format_number(ntohs(ip_header->src_ip), "IP"));
    printf("| %s ",
           print_human_format_number(ntohs(ip_header->dest_ip), "IP"));
    printf("| %s |",
            print_human_format_number(ntohs(udp_header->port), "UDP"));
    printf("\n++++++++++++++++++++++++++++++++++++++++++++\n");

    get_pattern(packet);
}

void print_routed_packet(struct sockaddr_in dest, char *result_if_name,
                         char *src_mac, char *dest_mac){
    /*
    printf("dest ip: %s | send interface : %s | dest mac : ",
           (unsigned char *)inet_ntoa(dest.sin_addr),
           result_if_name);
    print_mac(dest_mac);
    printf(" | src mac : ");
    print_mac(src_mac);
    printf("\n");
    */
}

void print_data_detail (unsigned char* data , int size)
{
    int i , j;
    for(i=0 ; i < size ; i++)
    {
        if( i!=0 && i%16==0)   //if one line of hex printing is complete...
        {
            fprintf(LOGFILE , "         ");
            for(j=i-16 ; j<i ; j++)
            {
                if(data[j]>=32 && data[j]<=128)
                    fprintf(LOGFILE , "%c",(unsigned char)data[j]); //if its a number or alphabet

                else fprintf(LOGFILE , "."); //otherwise print a dot
            }
            fprintf(LOGFILE , "\n");
        }

        if(i%16==0) fprintf(LOGFILE , "   ");
            fprintf(LOGFILE , " %02X",(unsigned int)data[i]);

        if( i== size-1)  //print the last spaces
        {
            for(j=0;j<15-i%16;j++)
            {
              fprintf(LOGFILE , "   "); //extra spaces
            }

            fprintf(LOGFILE , "         ");

            for(j=i-i%16 ; j<=i ; j++)
            {
                if(data[j]>=32 && data[j]<=128)
                {
                  fprintf(LOGFILE , "%c",(unsigned char)data[j]);
                }
                else
                {
                  fprintf(LOGFILE , ".");
                }
            }

            fprintf(LOGFILE ,  "\n" );
        }
    }
}

char* print_human_format_number(uint16_t num, char *type)
{
    if (strcmp(type, "UDP") == 0) {
        switch (num) {
            case DATA_PORT:
                return "DATA";
            case NACK_PORT:
                return "NACK";
        }
    } else if (strcmp(type, "ETHERNET") ==0) {
        switch (num) {
            case ROUTER_MAC:
                return "ROUTER_MAC";
            case NODE1_MAC:
                return "NODE1_MAC";
            case NODE2_MAC:
                return "NODE2_MAC";
            case NODE3_MAC:
                return "NODE3_MAC";
        }
    } else if (strcmp(type, "IP") ==0) {
        switch (num) {
            case NODE1_IP:
                return "NODE1_IP";
            case NODE2_IP:
                return "NODE2_IP";
            case NODE3_IP:
                return "NODE3_IP";
        }
    }
    return "";
}
