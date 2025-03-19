#!/bin/sh
light=${1:-100}
echo $light > /sys/class/backlight/pwm_leds/brightness
