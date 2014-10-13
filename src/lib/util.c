#include "util.h"
#include "print_packet.h"
#include <net/if.h>
#include <sys/ioctl.h>
#include <unistd.h>

bool is_nack_list_empty() {
    if ((globals.nackl).num_members == 0) {
        return true;
    } else {
        return false;
    }
}

bool is_last_packet_recieved() {
    return globals.last_bit_arrived;
}

void set_pattern(unsigned char *packet, uint64_t pttn_num)
{
    struct pattern *pttn = (struct pattern*)packet;
    pttn->value = htobe64(pttn_num);
}

uint64_t get_pattern(unsigned char *packet)
{
    struct pattern *pttn = (struct pattern*)packet;
    //printf("Pattern = %" PRId64 "\n", be64toh(pttn->value));
    return be64toh(pttn->value);
}

void create_log_file(){
    LOGFILE = fopen("log.txt","w");
    if (LOGFILE == NULL)
    {
        printf("Unable to create log.txt file.");
    }
}

int set_promisc(char *interface, int sock ) {
    struct ifreq ifr;
    strncpy(ifr.ifr_name, interface,strlen(interface)+1);
    if((ioctl(sock, SIOCGIFFLAGS, &ifr) == -1)) {
        /*Could not retrieve flags for the
        * interface*/
        perror("Could not retrive flags for the interface");
        exit(0);
    }
    //printf("DEBUG: The interface is ::: %s\n", interface);
    //perror("DEBUG: Retrieved flags from interface successfully");

    /*now that the flags have been
    * retrieved*/
    /* set the flags to PROMISC */
    ifr.ifr_flags |= IFF_PROMISC;
    if (ioctl (sock, SIOCSIFFLAGS, &ifr) == -1 ) {
        perror("Could not set the PROMISC flag:");
        exit(0);
    }
    //printf("DEBUG: Setting interface ::: %s ::: to promisc\n", interface);
    return(0);
}
