#!/bin/bash

NODES=("node1" "node2" "node3" "node4" "node5")
LOGFILE="checker.log"

# ログファイルを初期化
> $LOGFILE

# ノード上の既存のiperf3プロセスを終了
for NODE in "${NODES[@]}"; do
    if docker exec $NODE pkill iperf3 2>/dev/null; then
        echo "Killed iperf3 processes on $NODE" >> $LOGFILE
    else
        echo "No iperf3 processes were running on $NODE" >> $LOGFILE
    fi
done

# 各ノードをサーバーとして動作させる
for SERVER in "${NODES[@]}"; do
    if docker exec -d $SERVER sh -c "iperf3 -s --json > /tmp/iperf3-logs 2>&1 &"; then
        echo "Started iperf3 server on $SERVER" >> $LOGFILE
    else
        echo "Failed to start iperf3 server on $SERVER" >> $LOGFILE
        continue
    fi

    # 他の全てのノードをクライアントとして動作させる
    for CLIENT in "${NODES[@]}"; do
        if [ "$SERVER" != "$CLIENT" ]; then
            if ! docker exec $CLIENT iperf3 -c $SERVER -b 60M -t 4; then
                echo "Failed to run iperf3 client on $CLIENT to connect to $SERVER" >> $LOGFILE
            fi
            sleep 2
        fi
    done
    
    SERVER_LOG=$(docker exec $SERVER cat /tmp/iperf3-logs 2>/dev/null)
    
    if [ ! -z "$SERVER_LOG" ]; then
        echo "-----------------------------------------" >> $LOGFILE
        echo "Server-side bandwidth measurements for $SERVER:" >> $LOGFILE
        echo "$SERVER_LOG" | jq '.end.sum_received.bits_per_second' >> $LOGFILE
    else
        echo "Failed to retrieve logs for $SERVER." >> $LOGFILE
    fi

    # Kill iperf3 server process
    docker exec $SERVER pkill iperf3
    sleep 2
done
