#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

pthread_mutex_t mutex;	//互斥量
pthread_cond_t cond;

void hander(void *arg)
{
	free(arg);
	(void)pthread_mutex_unlock(&mutex);
}

void *thread1(void *arg)
{
	pthread_cleanup_push(hander,  &mutex);

	while(1) {
		printf("thread1 is running\n");
		pthread_mutex_lock(&mutex);					// 先加锁
		pthread_cond_wait(&cond, &mutex);			// 此函数会先将此线程加入条件变量改变队列，然后将mutex互斥锁解锁，直到条件状态改变，返回的时候再做加锁
		printf("thread1 applied the condition\n");
		pthread_mutex_unlock(&mutex);
		sleep(4);
	}

	pthread_cleanup_pop(0);
}

void *thread2(void *arg)
{
	while(1) {
		printf("thread2 is running\n");
		pthread_mutex_lock(&mutex);
		pthread_cond_wait(&cond, &mutex);
		printf("thread2 applied the condition\n");
		pthread_mutex_unlock(&mutex);
		sleep(1);
	}
}

int main()
{
	pthread_t thid1, thid2;

	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&cond, NULL);

	pthread_create(&thid1, NULL, thread1, NULL);
	pthread_create(&thid2, NULL, thread2, NULL);

	sleep(1);

	do {
		pthread_cond_signal(&cond);
		sleep(5);
	} while(1);

	// sleep(20);
	pthread_exit(0);

	return 0;
}

/*
 * 利用线程间共享的全局变量进行同步的一种机制。条件变量上的基本操作有：触发条件(当条件变为 true 时)；等待条件，挂起线程直到其他线程触发条件。
 */
