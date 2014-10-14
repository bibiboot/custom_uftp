#!/bin/bash

# Scripts for node3

# node3 sends file to node1 [ra31]
sudo ./bin/a etc/data/data.bin 1:etc/data/recv.bin 3 > log_a_31.txt &

# node3 sends file to node2 [ra32]
sudo ./bin/a etc/data/data.bin 2:etc/data/recv.bin 3 > log_a_32.txt &

# node3 recv file from node2 [rb23]
sudo ./bin/b 3 2 > log_b_23.txt &

# node3 recv file from node1 [rb13]
sudo ./bin/b 3 1 > log_b_13.txt &
