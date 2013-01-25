#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
 #include <unistd.h>


#define LENGTH 2048

int main(int argc, char **argv)
{
	char buf1[LENGTH];
	char buf2[LENGTH];
	int fd1, fd2;
	int ret;
	fd1 = open("hello.txt",O_RDONLY);
	read(fd1,buf1,LENGTH);
	printf("%s\n", buf1);
	
	strcpy(buf2, buf1);
	
	fd2 = open("new_file.txt",O_CREAT, O_RDWR);
	if (fd2 < 0) {
		printf("open - error\n");
		return 0;
	}
	ret = write(fd2, buf2, LENGTH);
	if (ret < 0) {
		printf("error\n");
		return 0;
	}

	printf("write is success\n");

	return 0;

}
