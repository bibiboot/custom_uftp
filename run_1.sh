#!/bin/bash

# Scripts for node1

# node1 sends file to node2 [ra12]
sudo ./bin/a etc/data/data.bin 2:etc/data/recv.bin 1 > log_a_12.txt &

# node1 sends file to node3 [ra13]
sudo ./bin/a etc/data/data.bin 3:etc/data/recv.bin 1 > log_a_13.txt &

# node1 recv file from node2 [rb21]
sudo ./bin/b 1 2 > log_b_21.txt &

# node1 recv file from node3 [rb31]
sudo ./bin/b 1 3 > log_b_31.txt &

