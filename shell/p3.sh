#!/bin/bash  

function pause(){  
read -n 1 -p "$*" INP  
if [[ $INP != '' ]] ; then  
	echo  'anykey'  
fi  
}  

pause 'Press any key to exit...'  

echo "coniture"

