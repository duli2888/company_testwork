#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
void handler(int sig)
{
	printf("Ctrl + C ---------------[handler]----Receive signal: %u\n", sig);
}

void my_wait(int signum)
{
	int status;
	wait(&status);
	printf("-----------[my_wait]----Receive signal:%d status:%d\n", signum, status);
}

void sigroutine(int num)
{
	switch(num)
	{
		case 1:
			printf("Ctrl + \\ ----------------[sigroutine]----SIGUP number:\n", num);
			break;
		case 2:
			printf("Ctrl + \\ ----------------[sigroutine]----SIGINT number:\n", num);
			break;
		case 3:
			printf("Ctrl + \\ ----------------[sigroutine]----SIGQUIT number:\n", num);
			break;
		default:
			break;
	}

	return;
}

int main(void)
{
	struct sigaction sa;
	int count;

	sa.sa_handler = handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;

	printf("task id is %d\n",getpid());

	sigaction(SIGINT, &sa, NULL);
	signal(SIGQUIT, sigroutine);

	while(1) {
		signal(SIGCHLD, my_wait);
		sigsuspend(&sa.sa_mask);
		printf("after sigsuspend\n");

		int pid;
		pid = fork();
		if (pid == 0) {
			printf("In child process\n");
			exit(1);
		}
		printf("loop\n");
	}

	return 0;
}

/*
 *  注释：
 *		(1) sigaction 的作用, 为什么传变量sa?
 *		(2) signal只是执行一次之后会变为默认的SIG_DF,为什么还会执行sigroutine?
 *		(3) sigsuspend 传 sa.sa_mask 的作用是什么?
 *		(4) 为什么会循环的执行sigroutine, sigsuspend对signal注册的入口函数为什么会有影响?
 */
