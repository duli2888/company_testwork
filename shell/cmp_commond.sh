#!/bin/bash

function cmp_file_print() {
     cmp $1 $2
     ret_val=$?				# $? 取值一次以后则丢失，变为零
	 if [ ${ret_val} -eq 1 ]; then
		 echo -e "\033[41;37;5m [不同] \033[0m"
	 fi
	 if [ "${ret_val}" -eq 0 ]; then
		 echo -e "\033[42;37;5m [相同] \033[0m"
	 fi
	 if [ "${ret_val}" -eq 2 ]; then
		 echo -e "\033[41;37;5m [文件不存在] \033[0m"
	 fi
}

cmp_file_print p1.sh p2.sh
