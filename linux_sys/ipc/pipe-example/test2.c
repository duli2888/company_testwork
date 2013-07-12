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

	if((pid = fork()) == 0) { // 子进程
		printf("\n");
//		close(fd[1]);   // 关闭写端
		sleep(2);       //确保父进程关闭写端
		num = read(fd[0], buf, 20);
		printf("read num is %d the data read from the pipe is %s\n", num, buf);
		exit(1);
	} else if(pid > 0) { // 父进程
//		close(fd[0]);//  关闭读端
		if(write(fd[1], "hello world", 20) != -1)
			printf("parent write over\n");
		sleep(5);
	}
	return 0;
}
/*
 * 注:可以不关闭pipe，pipe的读端在父进程和子进程中是相同的，写端也是同样，所以可以理解为子进程复制了父进程的
 * pipe。
 * 关于进程和线程，fork一个进程出来，两个进程的代码是一致的，所以代码是完全复制一份出来，变量也是一致的。
 * pthread_create()是创建一个线程出来，两个线程共享代码段，所以变量是共享的。
 */
