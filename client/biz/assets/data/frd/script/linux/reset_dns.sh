#!/bin/sh
DNS1=$1
DNS2=$2
GATEWAY=$(ip route | grep "^default" | sort -k5 -n | head -n 1 | awk '{print $3}')

truncate -s 0 /etc/resolv.conf
if [ ! -z "$DNS1" ]; then
    echo "nameserver $DNS1" >> /etc/resolv.conf
fi
if [ ! -z "$DNS2" ]; then
    echo "nameserver $DNS2" >> /etc/resolv.conf
fi
echo "nameserver $GATEWAY" >> /etc/resolv.conf