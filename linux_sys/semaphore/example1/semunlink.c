#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>

int main(int argc, char **argv) {
	if (argc != 2)
		printf("usage: semunlink <name>");

	sem_unlink(argv[1]);

	exit(0);
}
