#!/bin/sh
vol=${1:-10}
amixer cset name='Master Playback Volume' $vol
