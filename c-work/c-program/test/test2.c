#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main()
{
	printf("run 'pmap %d ' \n",getpid());

	pause();

}
