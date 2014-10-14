#!/bin/bash

# Scripts for node2

# node2 sends file to node1 [ra21]
sudo ./bin/a etc/data/data.bin 1:etc/data/recv.bin 2 > log_a_21.txt &

# node2 sends file to node3 [ra23]
sudo ./bin/a etc/data/data.bin 3:etc/data/recv.bin 2 > log_a_23.txt &

# node2 recv file from node1 [rb21]
sudo ./bin/b 2 1 > log_b_21.txt &

# node2 recv file from node3 [rb32]
sudo ./bin/b 2 3 > log_b_32.txt &
