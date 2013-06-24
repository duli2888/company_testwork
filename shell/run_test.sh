#!/bin/bash

function inside_op() {
cd td/orig_directory/
file_list=`echo *`

for file_name in $file_list; do
	# 取文件后缀
	file_type=`echo ${file_name} | awk -F '.' '{ print $5 $6 }'`

	not_do=`echo ${file_name} | awk -F '.' '{ print $1 $2 $3 $4}'`	# 测试版本不新的时候，解包解压等跳过去
	if [ ${not_do} = "app20130301" ]; then 
		echo "The version of this package is not unpack"
		rm ${file_name}
		continue
	fi

	case $file_type in 
		bin)
			new_name=`echo ${file_name} | awk -F '.' '{ print $1 ".bin" }'`
			mv ${file_name} ${new_name}
			;;
		bingz)
			gzip -d ${file_name}
			old_name=`echo ${file_name} | sed 's/\.[^\.]*$//'`
			new_name=`echo ${file_name} | awk -F '.' '{ print $1 ".bin" }'`
			mv ${old_name} ${new_name}
			;;
		tar|targz)
			tar xvf ${file_name}
			rm ${file_name}
			;;
	esac
done
cd ../..
}

function outside_op() {
mkdir -p td

cd $1
pkg_name=`echo *`
cd ../..

for pkg_name in $pkg_name; do
	rm -rf tmp/*
	rm -rf td/*
	echo "-----------------------[${pkg_name}]----------------------------------------------"
	out_file_type=`echo ${pkg_name} | awk -F '.' '{ print $5 $6 }'`
	case $out_file_type in 
		bin)
			not_do=`echo ${pkg_name} | awk -F '.' '{ print $1 $2 $3 $4}'`	# 测试版本不新的时候，解包解压等跳过去
			if [ ${not_do} = "app20130301" ]; then 
				echo "The version of this package is not unpack"
				continue	
			fi
			new_name=`echo ${pkg_name} | awk -F '.' '{ print $1 ".bin" }'`
			cp $1/${pkg_name}  td/${new_name}
			new_path=td
			;;
		bingz)
			not_do=`echo ${pkg_name} | awk -F '.' '{ print $1 $2 $3 $4}'`	# 测试版本不新的时候，解包解压等跳过去
			if [ ${not_do} = "app20130301" ]; then 
				echo "The version of this package is not unpack"
				continue	
			fi
			old_name=`echo ${pkg_name} | sed 's/\.[^\.]*$//'`
			new_name=`echo ${pkg_name} | awk -F '.' '{ print $1 ".bin" }'`
			gunzip -c $1/${pkg_name} > td/${new_name}
			#cp $1/${old_name} td/${new_name}
			new_path=td
			;;
		tar)
			tar xvf $1/${pkg_name} -C td
			t_dir=`echo ${pkg_name} | awk -F '.' '{ print $1 }'`

			if [ ${t_dir} = "pack" ]; then 
				inside_op 
				new_path=td/orig_directory
			else
				new_path=td/orig_directory
			fi
			;;
		targz)
			tar zxvf $1/${pkg_name} -C td
			new_path=td/orig_directory
		#	t_dir=`echo ${pkg_name} | awk -F '.' '{ print $1 }'`

		#	if [ ${t_dir} = "pack" ]; then 
		#		inside_op 
		#		new_path=td/orig_directory
		#	else
		#		new_path=td/orig_directory
		#	fi
			;;
	esac

	echo "开始解包${pkg_name} ..."
	../../out/hi3716/main prefix_pack/pack_directory/${pkg_name}

	echo "开始比对${pkg_name} ..."
	./cmpdir tmp ${new_path}
	sleep 5 
done
}

if [ $# -eq 2 ]; then
	TASK_DIR=$1	
else
	TASK_DIR=prefix_pack/pack_directory	
fi

for loop in {1..10}
do
	echo -e "\033[42;37;5m===========================[第${loop}遍测试]=======================================\033[0m"
	cd prefix_pack
	./make_file.sh
	cd ..
	outside_op ${TASK_DIR}
done

rm -rf prefix_pack/orig_director prefix_pack/pack_directory
