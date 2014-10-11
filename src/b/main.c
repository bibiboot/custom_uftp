#include <stdio.h>
#include "globals.h"
#include "hashl.h"
#include "mmap.h"
#include "list.h"
#include "parser.h"
#include "packetize.h"
#include "conn_b.h"
#include "sender_b.h"
#include "reciever_b.h"

void init(){

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

    // Create socket connection
    printf("[DEBUG] Connection setup\n");
    reciever_conn_setup();
    sender_conn_setup();
}

void start(){
    void *val;

    pthread_create(&globals.sen_th, 0, reciever, val);
    pthread_create(&globals.rev_th, 0, sender, val);
}

void cmd_parser(int argc, char *argv[])
{
    if (argc != 2) {
        printf("[SUMMARY] Error in command line parsing\n");
        exit(1);
    }

    strcpy(globals.hostname_a, argv[1]);
}

int main(int argc, char *argv[]){

    cmd_parser(argc, argv);

    init();

    start();

    pthread_join(globals.sen_th, NULL);
    pthread_join(globals.rev_th, NULL);
    printf("[SUMMARY] Exiting the program\n");
}
