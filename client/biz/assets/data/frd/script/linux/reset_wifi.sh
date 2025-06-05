#!/bin/sh

# Function to disconnect WiFi
wifi_disconnect() {
    # Stop services
    wpa_cli -i wlan0 disconnect 2>/dev/null
    killall wpa_supplicant 2>/dev/null
    killall udhcpc 2>/dev/null

    # Remove IP address and take interface down
    ip addr flush dev wlan0 2>/dev/null
    ip link set wlan0 down 2>/dev/null

    # Remove default route
    ip route del default via 0.0.0.0 dev wlan0 2>/dev/null

    # Turn off WiFi power
    dbus-send --system --print-reply --dest=rockchip.dbserver / rockchip.dbserver.net.Cmd \
        string:'{"table": "NetworkPower", "key": {"sType": "wifi"}, "data": {"iPower": 0}, "cmd": "Update"}'

    echo "WiFi disconnected"
}

# Function to connect to WiFi with DHCP
wifi_connect_dhcp() {
    local WIFISSID=$1
    local WIFIPWD=$2
    local CONF=/data/cfg/wpa_supplicant.conf

    # First ensure WiFi is fully disconnected
    wifi_disconnect

    # Enable WiFi power through dbus
    dbus-send --system --print-reply --dest=rockchip.dbserver / rockchip.dbserver.net.Cmd \
        string:'{"table": "NetworkPower", "key": {"sType": "wifi"}, "data": {"iPower": 1}, "cmd": "Update"}'

    # Create configuration file directly without template
    cat > $CONF << EOF
ctrl_interface=/var/run/wpa_supplicant
ap_scan=1
update_config=1
network={
        ssid="$WIFISSID"
        psk="$WIFIPWD"
        key_mgmt=WPA-PSK
}
EOF

    # Start wpa_supplicant
    wpa_supplicant -B -i wlan0 -c $CONF

    echo "nameserver 8.8.8.8" > /etc/resolv.conf

    # Start DHCP client
    udhcpc -i wlan0 &

    echo "WiFi connected to SSID: $WIFISSID with DHCP enabled"
}

# Function to connect to WiFi with static IP
wifi_connect_static() {
    local WIFISSID=$1
    local WIFIPWD=$2
    local IP=$3
    local NETMASK=$4
    local GATEWAY=$5
    local DNS1=$6
    local DNS2=$7
    local CONF=/data/cfg/wpa_supplicant.conf

    # First ensure WiFi is fully disconnected
    wifi_disconnect

    # Enable WiFi power through dbus
    dbus-send --system --print-reply --dest=rockchip.dbserver / rockchip.dbserver.net.Cmd \
        string:'{"table": "NetworkPower", "key": {"sType": "wifi"}, "data": {"iPower": 1}, "cmd": "Update"}'

    # Create configuration file directly without template
    cat > $CONF << EOF
ctrl_interface=/var/run/wpa_supplicant
ap_scan=1
update_config=1
network={
        ssid="$WIFISSID"
        psk="$WIFIPWD"
        key_mgmt=WPA-PSK
}
EOF

    # Start wpa_supplicant
    wpa_supplicant -B -i wlan0 -c $CONF

    # Wait for interface to become available
    sleep 3

    # Configure static IP
    ip addr flush dev wlan0
    ip addr add $IP/$NETMASK dev wlan0
    ip link set wlan0 up
    ip route add default via $GATEWAY

    # Configure DNS
    mkdir -p /etc/resolv.conf.d
    cat > /etc/resolv.conf << EOF
nameserver $DNS1
EOF

    # Add optional second DNS if provided
    if [ ! -z "$DNS2" ]; then
        echo "nameserver $DNS2" >> /etc/resolv.conf
    fi

    echo "WiFi connected to SSID: $WIFISSID with static IP: $IP"
}

# Function to check WiFi status
wifi_status() {
    echo "WiFi Status:"
    echo "------------"

    # Check interface status
    echo "Interface status:"
    ip addr show wlan0 2>/dev/null || echo "wlan0 interface not found"

    # Check routes
    echo -e "\nRoute information:"
    ip route | grep wlan0

    # Check DNS configuration
    echo -e "\nDNS configuration:"
    cat /etc/resolv.conf 2>/dev/null || echo "No DNS configuration found"

    # Check if wpa_supplicant is running
    echo -e "\nServices:"
    if pgrep -f "wpa_supplicant.*wlan0" > /dev/null; then
        echo "wpa_supplicant: Running"
    else
        echo "wpa_supplicant: Not running"
    fi

    # Check if DHCP client is running
    if pgrep -f "udhcpc.*wlan0" > /dev/null; then
        echo "DHCP client: Running"
    else
        echo "DHCP client: Not running"
    fi
}

# Display usage information
show_usage() {
    echo "Usage:"
    echo "  $0 on <SSID> <PASSWORD>                               # Connect with DHCP"
    echo "  $0 on-static <SSID> <PASSWORD> <IP> <NETMASK> <GATEWAY> <DNS1> [DNS2]  # Connect with static IP"
    echo "  $0 off                                                # Disconnect WiFi"
    echo "  $0 status                                             # Show WiFi status"
    echo
    echo "Examples:"
    echo "  $0 on \"My WiFi\" \"password123\"                        # Connect with DHCP"
    echo "  $0 on-static \"My WiFi\" \"password123\" 192.168.1.100 24 192.168.1.1 8.8.8.8 8.8.4.4  # Connect with static IP"
}

# Main script logic
case "$1" in
    "on")
        if [ -z "$2" ] || [ -z "$3" ]; then
            show_usage
            exit 1
        fi
        wifi_connect_dhcp "$2" "$3"
        ;;
    "on-static")
        if [ -z "$2" ] || [ -z "$3" ] || [ -z "$4" ] || [ -z "$5" ] || [ -z "$6" ] || [ -z "$7" ]; then
            show_usage
            exit 1
        fi
        wifi_connect_static "$2" "$3" "$4" "$5" "$6" "$7" "$8"
        ;;
    "off")
        wifi_disconnect
        ;;
    "status")
        wifi_status
        ;;
    *)
        show_usage
        exit 1
        ;;
esac

exit 0