#!/bin/bash

FILE_DIR=orig_directory   # 等号两边不能有空格
PACK_DIR=pack_directory

echo "正在创建测试用文件，請稍候... ..."

version_number=`date +"%Y.%m.%d"`

FILE_TYPE=`echo $RANDOM % 3 | bc`	# 确定文件类型

function get_file_size() {
	rand=`echo $RANDOM % 20 + 1| bc`			# 生成隨機數1～20

	case $rand in
		1|2|3|4|5|6|7|8)
			FILE_SIZE=0											# 40%, 0
			;;
		9|10)
			FILE_SIZE=`echo $RANDOM % 512 + 1 | bc`				# 10%, 1 ~ 512
			;;
		11|12)
			FILE_SIZE=`echo $RANDOM % 512 + 513 | bc`			# 10%, 513 ~ 1024
			;;
		13|14|15)
			FILE_SIZE=`echo $RANDOM % 10240 + 1025 | bc`		# 15%, 1025 ~ 11k
			;;
		16|17|18)
			FILE_SIZE=`echo $RANDOM % 1038336 + 10241 | bc`		# 15%, 11k ~ 1M
			;;
		19|20)
			FILE_SIZE=`echo $RANDOM % 30408704 + 1048577 | bc`	# 15%, 11k ~ 1M
			;;
	esac
}

get_file_size

#----------------------------------------------------------------------------------
# 只是为生成包的大小和类型做测试
# FILE_TYPE=2
# FILE_SIZE=2048k
#----------------------------------------------------------------------------------

#echo "FILE_SIZE = ${FILE_SIZE}"

# 创建.bin和file的文件	
function file_file_bin() {
	case $FILE_TYPE in
		0 )
			ORIG_NAME=/dev/urandom	
			;;
		1 )
			ORIG_NAME=/dev/zero	
			;;
		2 )
			ORIG_NAME=../inter_file
			;;
	esac
	echo "${ORIG_NAME}"
	./../mkfile -i ${ORIG_NAME} -o file1 -s $FILE_SIZE
	./../mkfile -i ${ORIG_NAME} -o file2 -s $FILE_SIZE
	./../mkfile -i ${ORIG_NAME} -o file3 -s $FILE_SIZE
	./../mkfile -i ${ORIG_NAME} -o file4 -s $FILE_SIZE
	./../mkfile -i ${ORIG_NAME} -o app.${version_number}.bin -s $FILE_SIZE
	cp app.${version_number}.bin app.2013.03.01.bin
	./../mkfile -i ${ORIG_NAME} -o kernel.${version_number}.bin -s $FILE_SIZE
	./../mkfile -i ${ORIG_NAME} -o loader.${version_number}.bin -s $FILE_SIZE
	./../mkfile -i ${ORIG_NAME} -o rootfs.${version_number}.bin -s $FILE_SIZE
}

# 创建.bin.gz文件
function file_bin_gz() {
	gzip -c app.${version_number}.bin > app.${version_number}.bin.gz
	gzip -c kernel.${version_number}.bin > kernel.${version_number}.bin.gz
	gzip -c loader.${version_number}.bin > loader.${version_number}.bin.gz
	gzip -c rootfs.${version_number}.bin > rootfs.${version_number}.bin.gz
}

# 创建.tar的文件
function file_tar() {
	tar cvf app.${version_number}.tar file*
	tar cvf rootfs.${version_number}.tar file*
}

# 创建.tar.gz的文件
function file_tar_gz() {
	tar czvf app.${version_number}.tar.gz file*
	tar czvf rootfs.${version_number}.tar.gz file*
}

##############################################################################################

mkdir -p ${FILE_DIR}
cd ${FILE_DIR}
##################################	创建第一级文件[单包]	###################################

# 创建.bin的文件	
file_file_bin

# 创建.bin.gz文件
file_bin_gz

# 创建.tar的文件
file_tar

# 创建.tar.gz的文件
file_tar_gz

##################################	创建第二级文件[多包]	###################################

cd ..
mkdir -p ${PACK_DIR}

sub_num=${version_number:8:2}

function new_pack_name() {
	pack_number=${version_number:0:8}
	if [ ${sub_num} -eq 1 ]; then
		sub_num=31
	fi
	((sub_num--))           # 如果今天的日期是很小的数字，则有会减为零或者负数
	if [ ${sub_num} -le 9 ]; then
		sub_num=0${sub_num}
	fi
	pack_number=${pack_number}${sub_num}
	pack_name=pack.${pack_number}.tar.gz
}

function new_pack_tar() {
	pack_name=${pack_name%.*z}
	tar cvf ${PACK_DIR}/${pack_name}  ${FILE_DIR}/loader.*.bin ${FILE_DIR}/kernel.*.bin.gz ${FILE_DIR}/rootfs.*.tar ${FILE_DIR}/app*.tar.gz
	pack_tar_size=`du -m ${PACK_DIR}/${pack_name}`
	pack_tar_size=`echo ${pack_tar_size} | awk -F ' ' '{ print $1 }'`
	if [ ${pack_tar_size} -gt 40 ]; then		# 如果包大于40M，则删除
		rm ${PACK_DIR}/${pack_name}
		tar cvf ${PACK_DIR}/${pack_name}   ${FILE_DIR}/kernel.*.bin.gz ${FILE_DIR}/rootfs.*.tar.gz
	fi
}


#	包中有多个.bin的文件"
new_pack_name
tar czvf ${PACK_DIR}/${pack_name} ${FILE_DIR}/*.bin

#	包中有多个.bin.gz文件
new_pack_name
tar czvf ${PACK_DIR}/${pack_name} ${FILE_DIR}/*.bin.gz
#new_pack_tar

#	包中有多个.tar文件
new_pack_name
tar czvf ${PACK_DIR}/${pack_name} ${FILE_DIR}/*.tar

#	包中有多个.tar.gz文件
new_pack_name
tar czvf ${PACK_DIR}/${pack_name} ${FILE_DIR}/*.tar.gz

#	包中有单个.bin，单个.bin.gz，单个.tar，单个.tar.gz文件
new_pack_name
tar czvf ${PACK_DIR}/${pack_name}  ${FILE_DIR}/loader.*.bin ${FILE_DIR}/kernel*.bin.gz ${FILE_DIR}/rootfs.*.tar ${FILE_DIR}/app*.tar.gz
#new_pack_tar

#	单个.bin，单个.bin.gz，单个.tar
new_pack_name
tar czvf ${PACK_DIR}/${pack_name} ${FILE_DIR}/app*bin ${FILE_DIR}/kernel*.bin.gz ${FILE_DIR}/rootfs.*.tar

#	单个.bin，单个.bin.gz，单个.tar.gz
new_pack_name
tar czvf ${PACK_DIR}/${pack_name} ${FILE_DIR}/app*bin ${FILE_DIR}/kernel*.bin.gz ${FILE_DIR}/app*.tar.gz
#new_pack_tar

#	单个.bin，单个.tar，单个.tar.gz
new_pack_name
tar czvf ${PACK_DIR}/${pack_name} ${FILE_DIR}/loader*bin ${FILE_DIR}/rootfs.*.tar ${FILE_DIR}/app*.tar.gz

#	单个.bin.gz，单个.tar，单个.tar.gz
new_pack_name
tar czvf ${PACK_DIR}/${pack_name} ${FILE_DIR}/kernel*.bin.gz ${FILE_DIR}/rootfs.*.tar ${FILE_DIR}/app*.tar.gz
# new_pack_tar

#	单个.tar.gz
new_pack_name
tar czvf ${PACK_DIR}/${pack_name} ${FILE_DIR}/app.*.tar.gz

#	单个.bin.gz
new_pack_name
tar czvf ${PACK_DIR}/${pack_name} ${FILE_DIR}/loader.*.bin.gz

##################################################################################3

function delete_big_file() {
file_list=`echo *`
for file_name in $file_list; do
	pack_tar_size=`du -m ${file_name}`
	pack_tar_size=`echo ${pack_tar_size} | awk -F ' ' '{ print $1 }'`
	if [ ${pack_tar_size} -gt 40 ]; then		# 如果包大于40M，则删除
		rm ${PACK_DIR}/${pack_name}
		tar cvf ${PACK_DIR}/${pack_name}   ${FILE_DIR}/kernel.*.bin.gz ${FILE_DIR}/rootfs.*.tar.gz
	fi
done
}

cd ${PACK_DIR}
delete_big_file
cd ..
