/* 
 * 1 在#include语句之后，我们定义函数原型与全局变量，然后我们进入main函数。在这里使用semget函数
 * 调用创建信号量，这会返回一个信号量 ID。如果程序是第一次调用（例如，使用一个参数并且argc > 1来调用），
 * 程序就会调用set_semvalue来初始化信号量并且将op_char设置为X。
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

//#include "semun.h"

static int set_semvalue(void);
static void del_semvalue(void);
static int semaphore_p(void);
static int semaphore_v(void);

static int sem_id;

int main(int argc, char **argv)
{
	int i;
	int pause_time;
	char op_char = 'O';

	srand((unsigned int)getpid());

	sem_id = semget((key_t)1234, 1, 0666 | IPC_CREAT);

	if(argc > 1) {
		if(!set_semvalue()) {
			fprintf(stderr, "Failed to initialize semaphore\n");
			exit(EXIT_FAILURE);
		}
		op_char = 'X';
		sleep(2);
	}
	/* 2 然后我们使用一个循环代码进入并且离开临界区10次。此时会调用semaphore_p函数，这个函数会设置信号量并且等待程序进入临界区。*/
		for(i=0;i<10;i++) {
			if(!semaphore_p()) exit(EXIT_FAILURE);
			printf("%c", op_char); fflush(stdout);
			pause_time = rand() % 3;
			sleep(pause_time);
			printf("%c", op_char); fflush(stdout);
			/* 3 在临界区之后，我们调用semaphore_v函数，在随机的等待之后再次进入for循环之后，将信号量设置为可用。在循环之后，调用del_semvalue来清理代码。*/
				if(!semaphore_v()) exit(EXIT_FAILURE);

			pause_time = rand() % 2;
			sleep(pause_time);
		}

	printf("/n%d - finished/n", getpid());

	if(argc > 1) {
		sleep(10);
		del_semvalue();
	}

	exit(EXIT_SUCCESS);
}

/* 4 函数set_semvalue在一个semctl调用中使用SETVAL命令来初始化信号量。在我们使用信号量之前，我们需要这样做。*/
static int set_semvalue(void)
{
	union semun sem_union;

	sem_union.val = 1;
	if(semctl(sem_id, 0, SETVAL, sem_union) == -1) return 0;
	return 1;
}

/* 5 del_semvalue函数几乎具有相同的格式，所不同的是semctl调用使用IPC_RMID命令来移除信号量ID：*/
static void del_semvalue(void)
{
	union semun sem_union;

	if(semctl(sem_id, 0, IPC_RMID, sem_union) == -1)
		fprintf(stderr, "Failed to delete semaphore/n");
}

/* 6 semaphore_p函数将信号量减1（等待）：*/
static int semaphore_p(void)
{
	struct sembuf sem_b;

	sem_b.sem_num = 0;
	sem_b.sem_op = -1;
	sem_b.sem_flag = SEM_UNDO;
	if(semop(sem_id, &sem_b, 1) == -1)
	{
		fprintf(stderr, "semaphore_p failed/n");
		return 0;
	}
	return 1;
}

/* 7 semaphore_v函数将sembuf结构的sem_op部分设置为1，从而信号量变得可用。*/
static int semaphore_v(void)
{
	struct sembuf sem_b;

	sem_b.sem_num = 0;
	sem_b.sem_op = 1;
	sem_b.sem_flag = SEM_UNDO;
	if(semop(sem_id, &sem_b, 1) == -1) {
		fprintf(stderr, "semaphore_v failed/n");
		return 0;
	}
	return 1;
}
