#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

int main(void)
{
	fd_set rfds, wfds;
	struct timeval tv;
	int retval;

	/* Watch stdin (fd 0) to see when it has input. */
	FD_ZERO(&rfds);
	FD_SET(0, &rfds);
	FD_SET(1, &wfds);

	/* Wait up to five seconds. */
	tv.tv_sec = 1;
	tv.tv_usec = 0;

	while(1) {
		FD_SET(0, &rfds);
		//retval = select(2, &rfds, &wfds, NULL, &tv);
		retval = select(1, &rfds, NULL, NULL, NULL);
		/* Don't rely on the value of tv now! */

		if (retval == -1)
			perror("select()");
		else if (retval)
			printf("Data is available now.\n");
		/* FD_ISSET(0, &rfds) will be true. */
		else
			printf("No data within five seconds.\n");

		FD_ZERO(&rfds);
	}

	exit(EXIT_SUCCESS);
}

/*
 * 如果有多个句柄集合，则返回大于零，如何知道是哪个句柄集合状态改变?
 * 如果一个句柄集合中，则有多个文件描述符状态改变，咋么区分?
 * 如果select返回大于零，则返回的值是文件描述符号改变的个数,在select之前，监视的文件描述符(FD_SET哪些文件描述符)，
 * 在select之后则通过FD_ISSET检查某个文件描述符是否改变
 *
 */

