#!/bin/bash

# Script for node1
FILE='etc/data/sync'

while :
do
        if [ -f $FILE ];
            then
            echo "File $FILE exists"
            break
        else
            echo "File $FILE does not exists"
        fi
done

start=`date +%s.%N`

#Transfer to node2
scp etc/data/data.bin 10.10.2.2:/tmp

#Transfer to node3
#scp etc/data/data.bin 10.10.2.3:/tmp

end=`date +%s.%N`

runtime=$(python -c "print(${end} - ${start})")
echo "Runtime was $runtime seconds"
