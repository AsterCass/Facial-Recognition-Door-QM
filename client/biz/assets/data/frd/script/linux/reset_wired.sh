#!/bin/sh

# 真dhcp
ethernet_connect_dhcp() {
    ethernet_dhcp
    ip link set eth0 up
    sleep 2
    udhcpc -i eth0 &
    echo "Ethernet connected with DHCP enabled"
}


# 假dhcp（本质上是断开网卡）
ethernet_dhcp() {
    killall udhcpc 2>/dev/null
    ip addr flush dev eth0 2>/dev/null
    ip link set eth0 down 2>/dev/null
    ip route del default via 0.0.0.0 dev eth0 2>/dev/null
    echo "Ethernet dhcp"
}

ethernet_connect_static() {
    local IP=$1
    local NETMASK=$2
    local GATEWAY=$3
    local DNS1=$4
    local DNS2=$5


    ethernet_dhcp


    ip link set eth0 up
    sleep 2
    ip addr flush dev eth0
    ip addr add $IP/$NETMASK dev eth0
    ip route add default via $GATEWAY


    truncate -s 0 /etc/resolv.conf
    if [ ! -z "$DNS1" ]; then
        echo "nameserver $DNS1" >> /etc/resolv.conf
    fi
    if [ ! -z "$DNS2" ]; then
        echo "nameserver $DNS2" >> /etc/resolv.conf
    fi
    echo "nameserver $GATEWAY" >> /etc/resolv.conf


    echo "Ethernet connected with static IP: $IP"
}

# Display usage information
show_usage() {
    echo "Usage:"
    echo "  $0 dhcp                                    # Connect with DHCP"
    echo "  $0 static <IP> <NETMASK> <GATEWAY> [DNS1] [DNS2]  # Connect with static IP"
    echo "  $0 status                                  # Show Ethernet status"
    echo
    echo "Examples:"
    echo "  $0 dhcp                                    # Connect with DHCP"
    echo "  $0 static 192.168.1.100 24 192.168.1.1    # Connect with static IP (default DNS)"
    echo "  $0 static 192.168.1.100 24 192.168.1.1 8.8.8.8 8.8.4.4  # Connect with custom DNS"
}

# Main script logic
case "$1" in
    "dhcp")
        ethernet_dhcp
        ;;
    "static")
        if [ -z "$2" ] || [ -z "$3" ] || [ -z "$4" ]; then
            show_usage
            exit 1
        fi
        ethernet_connect_static "$2" "$3" "$4" "$5" "$6"
        ;;
    *)
        show_usage
        exit 1
        ;;
esac

exit 0