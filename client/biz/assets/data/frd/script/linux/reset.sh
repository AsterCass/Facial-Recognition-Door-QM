#!/bin/bash

DATA_BK_FILE="/data/frd/script/linux/common.origin.bk.db"
DATA_FILE="/data/frd/db/common.db"


if [ ! -f "$COUNTER_FILE" ]; then
  echo "Error: File '$DATA_BK_FILE' not exist"
  exit 1
fi

rm -rf /data/frd/app-*

rm -rf /data/frd/db/*
rm -rf /data/frd/face/*

cp $DATA_BK_FILE $DATA_FILE

/etc/init.d/S99zplay stop
reboot