#!/bin/bash

version_number=`date +"%Y.%m.%d"`

file_name=pack.${version_number}.bin

echo $file_name

#	用一个全局变量控制生成pack.${version_number}.tar.gz文件的文件名不同
#	参数1:app	参数2:kernel	参数3:rootfs	参数4:loader	
#	1:bin 2:bin.gz 3:tar 4:tar.gz 0:没有
function pack_tar_gz() {
	file_app="app"
	file_kernel="kernel"
	file_rootfs="rootfs"
	file_loader="loader"
}

t_dir=`echo ${file_name} | awk -F '.' '{ print $1 }'`
if [ ${t_dir} = "pack" ]; then 
	echo "------------PACK------------"
fi





# 用时间月日.时.分作为版本号
