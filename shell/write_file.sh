#!/bin/bash

if [ $# -eq 1 ]; then
	loop_max=$1
else
	loop_max=100;
fi
loop=1
while [ $loop -le $loop_max ]
do
echo "hello Novel-supertv!!!hello Novel-supertv!!!hello Novel-supertv!!!hello Novel-supertv!!!hello Novel-supertv!!!hello Novel-supertv!!!"
loop=`expr $loop + 1`
done
