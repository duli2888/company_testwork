#include <sys/time.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>

int main ()
{
	int keyboard;
	int ret, i;
	char c;
	fd_set readfd;
	struct timeval timeout;
	keyboard = open("/dev/tty", O_RDONLY | O_NONBLOCK);		// keyboard = 3;
	assert(keyboard > 0);

	while(1) {
		timeout.tv_sec = 1;
		timeout.tv_usec = 0;

		FD_ZERO(&readfd);
		FD_SET(keyboard, &readfd);

		ret = select(keyboard + 1, &readfd, NULL, NULL, &timeout);

		if(FD_ISSET(keyboard, &readfd)) {
			i = read(keyboard, &c, 1);
			if('\n' == c)
				continue;
			printf("[input] --- %c\n", c);

			if ('q' == c)
				break;
		}
	}
}

/*
 * 如果一次性输入多个字符，则select是如何检测文件描述符状态改变的
 * 特别是一次只有一个字符输出，select是如何判断出来的,并作返回的
 */
