#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>        /* For mode constants */


int main(int argc, char **argv) {
	int c, flags;
	sem_t *sem;
	unsigned int value;

	flags = O_RDWR | O_CREAT;
	value = 1;
	while ((c = getopt(argc, argv, "ei:")) != -1) {
		switch (c) {
			case 'e':
				flags |= O_EXCL;
				break;

			case 'i':
				value = atoi(optarg);
				break;
		}
	}
	if (optind != argc - 1)
		printf("usage: semcreate [ -e ] [ -i initalvalue ] <name>");

	sem = sem_open(argv[optind], flags, S_IRUSR|S_IWUSR, value);
	printf("%d\n",sem);
	sem_close(sem);			// sem_close
	exit(0);
}

/* 一个进程终止时，内核还对其上仍然打开着的所有有名信号灯自动执行这样的信号灯关闭操作。不论该进程是自愿终止的还是非自愿终止的，这种自动关闭都会发生。
 * 但应注意的是关闭一个信号灯并没有将他从系统中删除。这就是说，Posix有名信号灯至少是随内核持续的：即使当前没有进程打开着某个信号灯，他的值仍然保持。
 */
