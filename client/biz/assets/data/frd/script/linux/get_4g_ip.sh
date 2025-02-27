thisIp=$(ip addr show ppp0 2>/dev/null | grep 'inet ' | awk '{print $2}' | cut -d/ -f1)
if [ -n "$thisIp" ]; then
    echo "$thisIp"
else
    echo ""
fi