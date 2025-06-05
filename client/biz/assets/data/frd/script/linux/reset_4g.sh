#!/bin/sh


fourG_disconnect() {

    poff simcom-pppd

    echo "nameserver 8.8.8.8" > /etc/resolv.conf

    echo "4G disconnected"
}


fourG_connect() {

    fourG_disconnect

    pppd call simcom-pppd &

    echo "nameserver 8.8.8.8" > /etc/resolv.conf

    echo "4G connected"
}


# Main script logic
case "$1" in
    "on")
        fourG_connect
        ;;
    "off")
        fourG_disconnect
        ;;
    *)
        exit 1
        ;;
esac

exit 0