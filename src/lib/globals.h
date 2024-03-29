#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <sys/time.h>
#include <pthread.h>

#include "uthash.h"
#include "config.h"
#include "cprotocol.h"
#include "my402list.h"
#include "color.h"

// Print function name, filename and line number in print
#define DEBUG(fmt, ...) printf("%s:%d: " fmt, __FILE__, __LINE__, __VA_ARGS__);
#define DBG(...) do{fprintf(stderr, "%s (%s, line %d): ", __func__, __FILE__, __LINE__); \
                         fprintf(stderr, __VA_ARGS__);           \
                         fprintf(stderr, "\n");} while(0)
#define DATA "DATA"
#define NACK "NACK"

#define DATA_PACKET 1
#define NACK_PACKET 2
#define DUMMY_PACKET 3

#define DATA_TOKEN '1'
#define DUMMY_TOKEN '2'
#define NACK_TOKEN '3'

#define DATA_TOKEN_LEN 1
#define LAST_TOKEN_LEN 1

#define PACKET_TYPE_LEN 1
#define SEQ_NUM_LEN 12
#define CHECKSUM_LEN 10

#define INF0 "eth0"

#define PACKET_LEN 65536
#define NUM_DUMMY_PACKETS 10

FILE *LOGFILE;

typedef long long unsigned int vlong;

// Hashmap data structure
typedef struct hashl {
    // Sequence number
    vlong seq_num;
    // Address of the node in list.
    My402ListElem *data_node_ptr;
    My402ListElem *nack_node_ptr;
    UT_hash_handle hh;
} hashed_link;

// Data list is stored as node below
// Both nack and data list have same type of node
struct node {
    vlong seq_num;
    // Its pointing to the start address of data
    char *mem_ptr;
    // Size in bits
    vlong size;
};

struct globals {
    struct config config;
    // Hashmap
    hashed_link *hashl;
    // Linked list
    // datal --> Data list
    My402List datal;
    // nackl --> Nack list
    My402List nackl;
    // Current maximum recieved seq num
    vlong current_seq;
    char recv_filename[100];
    // The source filename provided at command line is stord here
    char filename[100];
    // Used at nodeB to check if dummy but is recieved or not
    bool last_bit_arrived;
    bool last_bit_send;
    struct timeval a_sender_start;
    struct timeval dummy_reached;
    struct timeval b_reciever_end;
    long long unsigned int total_retrans;
    long long unsigned int total_nack_recv;

    pthread_t sniff_th, sender_th;
    uint16_t own_node, other_node;
    int send_sock_fd;
};

extern struct globals globals;

unsigned int time_diff_micro(struct timeval end, struct timeval start);
unsigned long long to_milli(struct timeval tv);
int send_nack_packet();
