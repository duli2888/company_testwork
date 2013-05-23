#!/bin/sh

ROOT_DIRECTORY=upgrade_directory   # 等号两边不能有空格
FILE_SIZE=4M

echo "创建视频网关多种测试文件包！！！"

function function_list() {
	echo "1 在目录下只有多个.bin的文件"
	echo "2 在目录下有单个.bin文件"
	echo "3 在目录下有多个.bin.gz文件"
	echo "4 在目录下有多个.bin.gz文件,单个.tar文件"
	echo "5 在目录下有多个.bin.gz文件,单个.tar.gz文件"
	echo "6 在目录下有多个.tar.gz文件"
	echo "7 在目录下有单个.tar.gz文件.单个.tar文件,单个.bin文件"
	echo "8 创建大文件,大文件的大小"
	echo "9 删除临时文件"
}

if [ "$1" = "c" ]; then
	echo "Clean All Package"
	rm pack.000f.8a.01.tar.gz
	exit 0
fi

function_list

read Keypress

case $Keypress in 
	1 )							# 在目录下只有多个.bin的文件	
		mkdir ${ROOT_DIRECTORY}
		sh write_file.sh | dd of=${ROOT_DIRECTORY}/app.000f.8a.01.bin
		sh write_file.sh | dd of=${ROOT_DIRECTORY}/rootfs.000f.8a.01.bin
		dd if=/dev/urandom of=${ROOT_DIRECTORY}/kernel.000f.8a.01.bin bs=1M count=1
		dd if=/dev/urandom of=${ROOT_DIRECTORY}/loader.000f.8a.01.bin bs=2M count=1
		tar czvf pack.000f.8a.01.tar.gz ${ROOT_DIRECTORY}  --remove-files 
		;;
	2 )							# 在目录下有单个.bin文件
		mkdir ${ROOT_DIRECTORY}
		dd if=/dev/urandom of=${ROOT_DIRECTORY}/rootfs.000f.8a.01.bin bs=2M count=1
		tar czvf pack.000f.8a.01.tar.gz ${ROOT_DIRECTORY}  --remove-files 
		;;
	3 )							# 在目录下有多个.bin.gz文件
		mkdir ${ROOT_DIRECTORY}
		dd if=/dev/urandom of=${ROOT_DIRECTORY}/app.000f.8a.01.bin bs=1M count=1
		gzip ${ROOT_DIRECTORY}/app.000f.8a.01.bin
		dd if=/dev/urandom of=${ROOT_DIRECTORY}/rootfs.000f.8a.01.bin bs=2M count=1
		gzip ${ROOT_DIRECTORY}/rootfs.000f.8a.01.bin
		dd if=/dev/urandom of=${ROOT_DIRECTORY}/kernel.000f.8a.01.bin bs=1M count=1
		gzip ${ROOT_DIRECTORY}/kernel.000f.8a.01.bin
		dd if=/dev/urandom of=${ROOT_DIRECTORY}/loader.000f.8a.01.bin bs=2M count=1
		gzip ${ROOT_DIRECTORY}/loader.000f.8a.01.bin
		tar czvf pack.000f.8a.01.tar.gz ${ROOT_DIRECTORY} --remove-files 
		;;
	4 )							# 在目录下有多个.bin.gz文件,单个.tar文件
		mkdir ${ROOT_DIRECTORY}
		sh write_file.sh | dd of=${ROOT_DIRECTORY}/app.000f.8a.01.bin 
		gzip ${ROOT_DIRECTORY}/app.000f.8a.01.bin
		sh write_file.sh | dd of=${ROOT_DIRECTORY}/loader.000f.8a.01.bin 
		gzip ${ROOT_DIRECTORY}/loader.000f.8a.01.bin
		sh write_file.sh | dd of=${ROOT_DIRECTORY}/kernel.000f.8a.01.bin 
		gzip ${ROOT_DIRECTORY}/kernel.000f.8a.01.bin
		mkdir ${ROOT_DIRECTORY}rootfs/
		sh write_file.sh | dd of=${ROOT_DIRECTORY}/loader/file1 
		sh write_file.sh | dd of=${ROOT_DIRECTORY}/loader/file2 
		sh write_file.sh | dd of=${ROOT_DIRECTORY}/loader/file3 
		tar cvf ${ROOT_DIRECTORY}/rootfs.0006.2a.01.tar ${ROOT_DIRECTORY}/rootfs --remove-files 
		tar czvf rootfs.000f.8a.01.tar.gz ${ROOT_DIRECTORY}  --remove-files 
		;;
	5 )							# 在目录下有多个.bin.gz文件,单个.tar.gz文件
		mkdir ${ROOT_DIRECTORY}
		sh write_file.sh | dd of=${ROOT_DIRECTORY}/app.000f.8a.01.bin 
		gzip ${ROOT_DIRECTORY}/app.000f.8a.01.bin
		sh write_file.sh | dd of=${ROOT_DIRECTORY}/loader.000f.8a.01.bin 
		gzip ${ROOT_DIRECTORY}/loader.000f.8a.01.bin
		sh write_file.sh | dd of=${ROOT_DIRECTORY}/kernel.000f.8a.01.bin 
		gzip ${ROOT_DIRECTORY}/kernel.000f.8a.01.bin
		mkdir ${ROOT_DIRECTORY}/loader
		sh write_file.sh | dd of=${ROOT_DIRECTORY}/loader/file1 
		sh write_file.sh | dd of=${ROOT_DIRECTORY}/loader/file2 
		sh write_file.sh | dd of=${ROOT_DIRECTORY}/loader/file3 
		tar czvf ${ROOT_DIRECTORY}/rootfs.0024.8e.1a.tar.gz ${ROOT_DIRECTORY}/rootfs  --remove-files 
		tar czvf rootfs.000f.8a.01.tar.gz ${ROOT_DIRECTORY}  --remove-files 
		;;
	6 )							# 在目录下有多个.tar.gz文件.
		mkdir ${ROOT_DIRECTORY}
		mkdir ${ROOT_DIRECTORY}/app
		sh write_file.sh | dd of=${ROOT_DIRECTORY}/app/file1
		sh write_file.sh | dd of=${ROOT_DIRECTORY}/app/file2
		sh write_file.sh | dd of=${ROOT_DIRECTORY}/app/file3
		tar cvf ${ROOT_DIRECTORY}/app.000f.8a.01.tar ${ROOT_DIRECTORY}/app  --remove-files 
		mkdir ${ROOT_DIRECTORY}/loader
		sh write_file.sh | dd of=${ROOT_DIRECTORY}/loader/file1
		sh write_file.sh | dd of=${ROOT_DIRECTORY}/loader/file2
		sh write_file.sh | dd of=${ROOT_DIRECTORY}/loader/file3
		tar czvf ${ROOT_DIRECTORY}/rootfs.000b.7a.02.tar.gz ${ROOT_DIRECTORY}/rootfs  --remove-files 
		tar czvf rootfs.000f.8a.01.tar.gz ${ROOT_DIRECTORY}  --remove-files 
		;;
	7 )							# 在目录下有单个.tar.gz文件.单个.tar文件,单个.bin文件
		mkdir ${ROOT_DIRECTORY}
		sh write_file.sh | dd of=${ROOT_DIRECTORY}/loader.000f.8a.01.bin
		gzip ${ROOT_DIRECTORY}/loader.000f.8a.01.bin
		mkdir ${ROOT_DIRECTORY}/app
		sh write_file.sh | dd of=${ROOT_DIRECTORY}/app/file1 
		sh write_file.sh | dd of=${ROOT_DIRECTORY}/app/file2
		sh write_file.sh | dd of=${ROOT_DIRECTORY}/app/file3
		tar cvf ${ROOT_DIRECTORY}/app.0035.81.05.tar ${ROOT_DIRECTORY}/app  --remove-files 
		mkdir ${ROOT_DIRECTORY}/rootfs
		sh write_file.sh | dd of=${ROOT_DIRECTORY}/loader/file1
		sh write_file.sh | dd of=${ROOT_DIRECTORY}/loader/file2
		sh write_file.sh | dd of=${ROOT_DIRECTORY}/loader/file3
		tar czvf ${ROOT_DIRECTORY}/rootfs.0012.06.04.tar.gz ${ROOT_DIRECTORY}/rootfs  --remove-files 
		tar czvf pack.000f.8a.01.tar.gz ${ROOT_DIRECTORY}  --remove-files 
		;;
	8 )							# 创建大文件,大文件的大小
		mkdir ${ROOT_DIRECTORY}
		dd if=/dev/urandom of=${ROOT_DIRECTORY}/loader.000f.8a.01.bin bs=${FILE_SIZE} count=1
		gzip ${ROOT_DIRECTORY}/loader.000f.8a.01.bin
		mkdir ${ROOT_DIRECTORY}/app
		dd if=/dev/urandom of=${ROOT_DIRECTORY}/app/file1 bs=${FILE_SIZE} count=1
		dd if=/dev/urandom of=${ROOT_DIRECTORY}/app/file2 bs=${FILE_SIZE} count=1
		dd if=/dev/urandom of=${ROOT_DIRECTORY}/app/file3 bs=${FILE_SIZE} count=1
		tar cvf ${ROOT_DIRECTORY}/app.001f.3e.11.tar ${ROOT_DIRECTORY}/app  --remove-files 
		mkdir ${ROOT_DIRECTORY}/rootfs
		dd if=/dev/urandom of=${ROOT_DIRECTORY}/loader/file1 bs=${FILE_SIZE} count=1
		dd if=/dev/urandom of=${ROOT_DIRECTORY}/loader/file2 bs=${FILE_SIZE} count=1
		dd if=/dev/urandom of=${ROOT_DIRECTORY}/loader/file3 bs=${FILE_SIZE} count=1
		tar czvf ${ROOT_DIRECTORY}/rootfs.002b.7b.02.tar.gz ${ROOT_DIRECTORY}/rootfs  --remove-files 
		tar czvf pack.000f.8a.01.tar.gz ${ROOT_DIRECTORY}  --remove-files 
		;;
esac

