#!/bin/bash

if [[ "$1" != "1" &&  "$1" != "0" ]]; then
	echo "Argument should be 0 or 1"
	exit 0 
fi

echo $1 > /sys/devices/system/cpu/cpu5/online
echo $1 > /sys/devices/system/cpu/cpu3/online
echo $1 > /sys/devices/system/cpu/cpu1/online
echo $1 > /sys/devices/system/cpu/cpu6/online
echo $1 > /sys/devices/system/cpu/cpu4/online
echo $1 > /sys/devices/system/cpu/cpu2/online
echo $1 > /sys/devices/system/cpu/cpu0/online 