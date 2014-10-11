#!/bin/bash

# Create 10Mb
dd if=/dev/urandom of=etc/data/data.bin bs=1M count=10
