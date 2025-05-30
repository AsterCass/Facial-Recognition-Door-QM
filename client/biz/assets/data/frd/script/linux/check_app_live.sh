#!/bin/bash

COUNTER_FILE="/data/frd/script/linux/live.txt"
PROCESS_NAME="FacialRecognitionDoor"

# 如果文件不存在，初始化为 0
if [ ! -f "$COUNTER_FILE" ]; then
  echo 0 > "$COUNTER_FILE"
fi

# 读取当前计数
COUNT=$(cat "$COUNTER_FILE")

# 判断是否达到延迟次数（10 次 = 10 分钟）
if [ "$COUNT" -lt 10 ]; then
  COUNT=$((COUNT + 1))
  echo "$COUNT" > "$COUNTER_FILE"
  exit 0
fi

# 10分钟后判活
if ! pgrep -f "$PROCESS_NAME" > /dev/null; then
    echo "[$(date)] Process not running. Restarting..."
    /etc/init.d/S99zplay stop
    reboot
else
    echo "[$(date)] Process is running."
fi


