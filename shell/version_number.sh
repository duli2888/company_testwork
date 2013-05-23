#!/bin/bash

version_number=`date +"%Y.%m.%d"`

sub_num=${version_number:8:2}
echo "[out]sub_number = ${sub_num}"

function new_pack_number() {
	pack_number=${version_number:0:8}
	echo "sub_num = ${sub_num}"
	((sub_num--))			# 如果今天的日期是很小的数字，则有会减为零或者负数
	pack_number=${pack_number}${sub_num}
	pack_name=pack.${pack_number}.tar.gz
}

new_pack_number
echo "pack_number[0] = ${pack_name}"

new_pack_number
echo "pack_number[1] = ${pack_name}"

new_pack_number
echo "pack_number[2] = ${pack_name}"

new_pack_number
echo "pack_number[3] = ${pack_name}"
