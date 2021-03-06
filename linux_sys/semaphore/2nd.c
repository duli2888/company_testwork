#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

// 获取字符串示例
char msg[32];
sem_t msg_ready;
sem_t msg_empty;
pthread_mutex_t msg_queue_mutex = PTHREAD_MUTEX_INITIALIZER;

void *getMessage(void *arg) {
	static msg_cnt = 0;
	char *messages[] = {
		"message----->1",
		"message----->2",
		"message----->3",
		"message----->4",
		"message----->5",
		"message----->n",
	};

	int i;
	for (i = 0; i < 10; ++i) {
		sem_wait(&msg_empty);

		// pthread_mutex_lock(&msg_queue_mutex);

		if (msg_cnt >= 6)
			msg[0] = '\0';
		else {
			strcpy(msg, messages[msg_cnt++]);
			printf("[GET]------[%s]\n", messages[msg_cnt]);
		}

		sem_post(&msg_ready);
		// pthread_mutex_unlock(&msg_queue_mutex);
		sleep(1);
	}

	return NULL;
}

void *showMessage(void *arg) {
	while (1) {
		sem_wait(&msg_ready);

		// pthread_mutex_lock(&msg_queue_mutex);

		printf("[SHOW]------[%s]\n", msg);
		sem_post(&msg_empty);

		// pthread_mutex_unlock(&msg_queue_mutex);

		if (msg[0] == '\0')
			return NULL;
	}
}

int main() {
	pthread_t tid1, tid2;

	strcpy(msg, "msg empty");

	if (sem_init(&msg_ready, 0, 0) != 0) {
		printf("sem_init() failed\n");
		return 1;
	}

	if (sem_init(&msg_empty, 0, 1) != 0) {
		printf("sem_init() failed\n");
		return 2;
	}

	if (pthread_create(&tid1, NULL, getMessage, NULL) != 0) {
		printf("pthread_create(&tid1) failed\n");
		return 3;
	}

	if (pthread_create(&tid2, NULL, showMessage, NULL) != 0) {
		printf("pthread_create(&tid2) failed\n");
		return 4;
	}

	pthread_join(tid1, NULL);
	pthread_join(tid2, NULL);

	return 0;
}



/*
 * 注释：(1) pthread_mutex_lock() 和pthread_mutex_unlock() 函数是保护多个线程访问的变量的控制，
 *			 只能保证上锁的区域在同一个时间，只能保证被同一个线程访问，不能保证多个县城交替执行，
 *			 所以有可能某个线程可以连续多次访问某个变量。
 *		 (2) sem_wait() 和 sem_post() 在两个线程中交叉使用，可以保证两个函数交替执行的问题。
 */
