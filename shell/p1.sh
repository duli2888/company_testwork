#!/bin/bash

function return_value() {
echo "return_value()"
}

echo "Program name is $0"
echo "parameter count is $#"
echo "$@"
echo "$$"

case $1 in
	1 )
		echo "hello 1"
		;;
	2 )
		echo "hello 2"
		;;
esac

trap "ls -l" EXIT
return_value
ls
echo $?
