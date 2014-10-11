#!/bin/bash

# Create 1Gb file
#dd if=/dev/urandom of=etc/data/data.bin bs=1M count=1000

# Create 1Mb
#dd if=/dev/urandom of=etc/data/data.bin bs=1M count=1

# Create 10Mb
dd if=/dev/urandom of=etc/data/data.bin bs=1M count=10

# Create 50Mb
#dd if=/dev/urandom of=etc/data/data.bin bs=1M count=50

# Create 1Kb
#dd if=/dev/urandom of=etc/data/data.bin bs=1K count=1
