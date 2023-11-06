#!/bin/bash

NODES=("node1" "node2" "node3" "node4" "node5")
LOGFILE="checkers.log"
> $LOGFILE

for NODE in "${NODES[@]}"; do
    case $NODE in
        "node2")
            docker exec $NODE sh -c 'cd ~/share/infoExpNetKadai/server && ./router node5 9.79 node1 2.08' >> $LOGFILE 2>&1 &
            ;;
        "node4")
            docker exec $NODE sh -c 'cd ~/share/infoExpNetKadai/router && ./router node1 9.94 node5 36.78' >> $LOGFILE 2>&1 &
            ;;
        "node5")
            docker exec $NODE sh -c 'cd ~/share/infoExpNetKadai/router && ./router node1 1' >> $LOGFILE 2>&1 &
            ;;
        *)
            ;;
    esac
done
jobs -p


# sever側
# ./server test.dat 4 41 2 9.89 1 1
# client 側
# ./client 172.21.0.30 10000 received.dat