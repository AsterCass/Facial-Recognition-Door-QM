#!/bin/sh
v4l2-ctl  -d /dev/video15 -c  exposure=$1,analogue_gain=$2