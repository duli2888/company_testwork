#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void handler(int sig)
{
	printf("------------------------\n Receive signal: %u\n", sig);
}

void sigroutine(int num)
{
	switch(num)
	{
		case 1:
			printf("SIGUP === signal\n");
			break;
		case 2:
			printf("SIGINT === signal\n");
			break;
		case 3:
			printf("SIGQUIT === signal\n");
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

	sigaction(SIGTERM, &sa, NULL);
	signal(SIGHUP, sigroutine);
	signal(SIGINT, sigroutine);
	signal(SIGQUIT, sigroutine);

	while(1)
	{
		sigsuspend(&sa.sa_mask);
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
