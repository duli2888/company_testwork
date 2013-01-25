#include <stdio.h>
#include <string.h>

#define LENGTH 2048

int main(int argc, char **argv)
{
	char buf1[LENGTH];
	char buf2[LENGTH];
	FILE *fd1, *fd2;
	int ret;
	fd1 = fopen("hello.txt","r");
	fread(buf1, 10, 10,fd1);
	printf("%s\n", buf1);
	
	strcpy(buf2, buf1);
	
	fd2 = fopen("new_file.txt","w");
	if (fd2 < 0) {
		printf("open - error\n");
		return 0;
	}
	ret = fwrite(buf2, 10, 10, fd2);
	if (ret < 0) {
		printf("fwrite - error\n");
		return 0;
	}

	printf("write is success\n");
	fclose(fd1);
	fclose(fd2);

	return 0;

}
