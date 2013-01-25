#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>


int fd[2];	//File descriptor for creating a pipe
int main() {
	pid_t pid;
	void *status;
	int result;
	char buf[20];
	int num;
	memset(buf, '\0', 20);

	result = pipe(fd);
	if (result < 0) {
		perror("pipe create is error");
		exit(1);
	}

	if(write(fd[1], "hello world", 20) != -1)
		printf("write over\n");

	sleep(2);
	num = read(fd[0], buf, 20);

	printf("buf = [%s]\n", buf);

	return 0;
}
