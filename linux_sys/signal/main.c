#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void handler(int sig)
{
	printf("Receive signal: %u\n", sig)	;
}


void sigroutine(int num)
{
	switch(num)
	{
		case 1:
			printf("SIGUP signal\n");
			break;
		case 2:
			printf("SIGINT signal\n");
			break;
		case 3:
			printf("SIGQUIT signal\n");
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











