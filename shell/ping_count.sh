#!/bin/bash

for i in {1..255}
do
	echo "the number of $i computer is "
	ping -c 1 192.168.6.$i
	if [ $? -eq 0 ]; then
		echo "192.168.6.$i" >> list
	fi
done

# 测试局域网中有多少台设备的网络可以ping通
