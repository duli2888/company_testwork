#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <semaphore.h>
#include <pthread.h>

void *thread_function(void *arg);		/* 线程入口函数 */
void print();						/* 共享资源函数 */
sem_t *sem;								/* 定义Posix有名信号灯 */
int val;								/* 定义信号灯当前值 */

int main(int argc, char *argv[])
{
	int n = 0;
	pthread_t tid;

	if(argc!=2) {
		printf("please input a file name!\n");
		exit(1);
	}

	sem = sem_open(argv[1], O_CREAT, 0644, 3);	/*打开一个信号灯*/

	while(n++) {			/*循环创建5个子线程，使他们同步运行*/
		if((pthread_create(&tid, NULL, thread_function, NULL)) != 0) {
			perror("Thread creation failed");
			exit(1);
		}
	}
	pthread_join(tid, NULL);
	sem_close(sem);
	sem_unlink(argv[1]);

	return 0;
}

void *thread_function(void *arg)
{
	sem_wait(sem);			/*申请信号灯*/
	print();				/*调用共享代码段*/
	sleep(1); 
	sem_post(sem);			/*释放信号灯*/
	printf("I’m finished, my tid is %d\n", pthread_self());
}

void print()
{
	printf("I get it, my tid is %d\n", pthread_self());
	sem_getvalue(sem, &val);
	printf("Now the value have %d\n", val);
}
