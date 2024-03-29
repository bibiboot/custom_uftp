#include <stdio.h>
#include "globals.h"
#include "hashl.h"
#include "mmap.h"
#include "list.h"
#include "parser.h"
#include "packetize.h"
#include "reciever_b.h"

void init()
{
    // 10MB
    // 10485760
    printf("Waiting for 10MB file\n");
    globals.config.total_size = 10485760;

    // Create data list
    printf("[DEBUG] Create DATA and NACK list\n");
    create_recv_list(&globals.datal, DATA);
    create_recv_list(&globals.nackl, NACK);

    strcpy(globals.recv_filename, "etc/data/recv.bin");
    printf("[DEBUG] Recieved filename : %s\n", globals.recv_filename);
}

void *sender(void *val)
{
    register_signal();
}

void start(){
    void *val;

    pthread_create(&globals.sniff_th, 0, reciever, val);
    pthread_create(&globals.sender_th, 0, sender, val);
}

void cmd_parser(int argc, char *argv[])
{
    if (argc != 3) {
        printf("[SUMMARY] Error in command line parsing\n");
        exit(1);
    }

    globals.own_node = atoi(argv[1]);
    globals.other_node = atoi(argv[2]);
}

int main(int argc, char *argv[]){

    cmd_parser(argc, argv);

    globals.send_sock_fd = get_socket();

    init();

    start();

    pthread_join(globals.sniff_th, NULL);
    pthread_join(globals.sender_th, NULL);
    printf("[SUMMARY] Exiting the program\n");
}
