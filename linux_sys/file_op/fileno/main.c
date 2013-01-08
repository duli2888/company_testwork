#include <stdio.h>
#include <string.h>

int main()
{
	int fd;
	FILE *fp;
	char buffer[20];
	memset(buffer, '\0', 20);
	fp = fopen("tmp", "w+");	
	if (fp == NULL) {
		printf("open() is error\n");
		return 0;
	}

	fwrite("hello fileno", 1, 13, fp);

	fseek(fp, 0, SEEK_SET); // 需要将文件偏移量置于文件首
	fd = fileno(fp);
	read(fd, buffer, 20);

	printf("%s\n", buffer);
	return 0;
}
