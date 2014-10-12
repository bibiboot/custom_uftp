#include <stdio.h>
#include "globals.h"
#include "hashl.h"
#include "mmap.h"
#include "list.h"
#include "parser.h"
#include "packetize.h"
#include "sender_a.h"
#include "reciever_a.h"
#include "socket_util.h"

void init(){

    // Create memory map files
    char *data_ptr = get_memory_map_ptr(globals.filename, &globals.config.total_size);
    printf("[DEBUG] Creating memory map of the file, size = %llu \n", globals.config.total_size);

    // Create data list
    printf("[DEBUG] Creating data list\n");
    create_list(data_ptr, &globals.datal, DATA);
}

int cmd_parser(int argc, char *argv[]){
    if (argc != 3) {
        return 1;
    }

    int num = sscanf(argv[2], "%[^:]:%s", globals.hostname_b, globals.recv_filename);
    if (num != 2) {
        return 1;
    }

    strcpy(globals.filename, argv[1]);
    globals.src_node = 1;
    globals.dest_node = 2;

    return 0;
}

void start(){
    gettimeofday(&globals.a_sender_start, NULL);

    void *val;
    pthread_create(&globals.sniff_th, 0, reciever, val);
    pthread_create(&globals.sender_th, 0, sender, val);
}

int main(int argc, char *argv[]){

    // Command line parsing
    if (cmd_parser(argc, argv) != 0) {
        DBG("Error in parsing command line");
    }
    printf("[SUMMARY] Source filename : %s, Dest filename : %s\n",
            globals.filename, globals.recv_filename);

    globals.send_sock_fd = get_socket();

    // Initilaization
    init();

    // Fork and start sending
    start();

    // Wait for both the childs to get over
    pthread_join(globals.sender_th, NULL);
    pthread_join(globals.sniff_th, NULL);
    printf("[SUMMARY] Program is exiting\n");
}
