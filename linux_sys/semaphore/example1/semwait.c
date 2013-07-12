#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>

int main(int argc, char **argv) {
	sem_t *sem;
	int val;

	if (argc != 2)
		printf("usage: semwait <name>");

	sem = sem_open(argv[1], 0);
	sem_wait(sem);
	sem_getvalue(sem, &val);
	printf("pid %ld has semaphore, value = %d\n", (long) getpid(), val);

	exit(0);
}
