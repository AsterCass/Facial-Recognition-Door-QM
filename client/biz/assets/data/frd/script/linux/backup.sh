#!/bin/bash

mkdir -p /tmp/frd
cp -rf /data/frd/bin /tmp/frd/bin
cp -rf /data/frd/db /tmp/frd/db
cp -rf /data/frd/dump /tmp/frd/dump
cp -rf /data/frd/log /tmp/frd/log
cp -rf /data/frd/script /tmp/frd/script
cp -rf /data/frd/log.txt /tmp/frd/log.txt

cd /tmp
tar -czvf frd.bk.tar.gz frd/bin frd/db frd/dump frd/log frd/script frd/log.txt
mv frd.bk.tar.gz /data/frd/
rm -rf /tmp/frd