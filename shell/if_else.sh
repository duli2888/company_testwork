#!/bin/bash

a=20

FILE_SIZE=`echo $RANDOM % $((20480 - 10240)) + 10240 | bc`
echo "FILE_SIZE = ${FILE_SIZE}"

if (( a == 21 ));then
	echo "a == 21"
elif (( 3 < a < 10 ));then
   echo " 3 < a < 10 "	
fi
