#!/bin/sh
interface_state=$(ip link show eth0 2>/dev/null | grep -o "NO-CARRIER" || echo "")
thisIp=$(ip addr show eth0 2>/dev/null | grep 'inet ' | awk '{print $2}' | cut -d/ -f1)
if [ -n "$interface_state" ] || [ -z "$thisIp" ] || [[ "$thisIp" == 169.254.* ]]; then
    echo ""
else
    echo "$thisIp"
fi