#!/bin/bash

#read -n1 -t 3 Key
#echo ${Key}

function interrupt() {
	#if [ "${Key}" = '' ];then
	#	echo "hello Key!!!!!!!!!!"
	#fi
	read -sn 1 KEY
	echo "hello Key!!!!!!!!!!"
}

interrupt
echo "quit shell"

