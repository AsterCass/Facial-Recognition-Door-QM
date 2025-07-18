#!/bin/bash


auto_reboot() {
  (echo -e "0 4 * * * sh /data/frd/script/linux/reboot_app.sh\n* * * * * sh /data/frd/script/linux/check_app_live.sh") | crontab -
}


disable_auto_reboot() {
  (echo -e "* * * * * sh /data/frd/script/linux/check_app_live.sh") | crontab -
}



case "$1" in
    "on")
        auto_reboot
        ;;
    "off")
        disable_auto_reboot
        ;;
    *)
        exit 1
        ;;
esac

exit 0