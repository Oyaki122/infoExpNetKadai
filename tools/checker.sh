#!/bin/bash

NODES=("node1" "node2" "node3" "node4" "node5")
LOGFILE="checkers.log"

# ログファイルを初期化
> $LOGFILE

# ノード上の既存のiperf3プロセスを終了
for NODE in "${NODES[@]}"; do
    if docker exec $NODE pkill iperf3 2>/dev/null; then
        echo "[$NODE] Killed iperf3 processes." >> $LOGFILE
    else
        echo "[$NODE] No iperf3 processes were running." >> $LOGFILE
    fi
done

# 各ノードをサーバーとして動作させる
for SERVER in "${NODES[@]}"; do
    if docker exec -d $SERVER sh -c "iperf3 -s --json > /tmp/iperf3-logs 2>&1 &"; then
        echo "[$SERVER] Started iperf3 server." >> $LOGFILE
    else
        echo "[$SERVER] Failed to start iperf3 server." >> $LOGFILE
        continue
    fi

    # 他の全てのノードをクライアントとして動作させる
    for CLIENT in "${NODES[@]}"; do
        if [ "$SERVER" != "$CLIENT" ]; then
            if ! docker exec $CLIENT iperf3 -c $SERVER -b 60M -t 4; then
                echo "[$CLIENT -> $SERVER] Failed to run iperf3 client." >> $LOGFILE
            else
                echo "[$CLIENT -> $SERVER] Successfully ran iperf3 client." >> $LOGFILE
            fi
            sleep 2
        fi
    done
    
    SERVER_LOG=$(docker exec $SERVER cat /tmp/iperf3-logs 2>/dev/null)
    
    if [ ! -z "$SERVER_LOG" ]; then
        echo "-----------------------------------------" >> $LOGFILE
        echo "[$SERVER] Server-side bandwidth measurements:" >> $LOGFILE
        echo "$SERVER_LOG" | jq '.end.sum_received.bits_per_second' >> $LOGFILE
    else
        echo "[$SERVER] Failed to retrieve logs." >> $LOGFILE
    fi

    # Kill iperf3 server process
    docker exec $SERVER pkill iperf3
    sleep 2
done
