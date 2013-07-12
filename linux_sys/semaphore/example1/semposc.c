#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
int main(int argc, char **argv) {
	sem_t *sem;
	int val;

	if (argc != 2)
		printf("usage: sempost <name>");

	sem = sem_open(argv[1], 0);
	sem_post(sem);
	sem_getvalue(sem, &val);
	printf("value = %d\n", val);

	exit(0);
}
