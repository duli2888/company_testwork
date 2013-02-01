#include <stdio.h>
#include <string.h>
#include <pthread.h>

#define EU_MAX_THREADS 10

int p_fd[2];
static int thread_cnt = 0;
static pthread_t saved_threads[HVG_EU_MAX_THREADS];
char g_ch;

void *manage_thread(pthread_t tid) {
	int i;
	for (i = 0 ; i < 24; i++) {
		g_ch = 'a' + i;
	}


}

int register_thread(pthread_t tid) { 
	if (thread_cnt < EU_MAX_THREADS) {
		saved_threads[thread_cnt++] = tid;
		return 0;
	}

	// 线程登记表已满
	PRINTF("thread register table is full\n");
	return -1;
}

int join_all_threads(void) {
	int i;

	PRINTF("waiting for %d threads to finish\n", thread_cnt);
	for (i = 0; i < thread_cnt; ++i)
		pthread_join(saved_threads[i], NULL);

	fflush(stdout);
	fflush(stderr);

	thread_cnt = 0;
	return 0;
}

//#define BUF_LEN 65536   
//#define BUF_LEN 655360
#define BUF_LEN 20

void *push_data(void *data)
{
	char *p_buf[BUF_LEN];
	memset(p_buf, ch, BUF_LEN);

	int count = write(p_fd[1], p_buf, BUF_LEN);
	printf("write count = %d\n",count);
	return NULL;
}

void *show_data(void *data)
{
	char s_buf[BUF_LEN];
	read(p_fd[0], s_buf, BUF_LEN);
	printf("%s\n", s_buf);
	return NULL;
}


int main()
{
	pthread_t tid1, tid2;

	int ret = pipe(p_fd);
	if (ret == -1) {
		printf("pipe error\n");
		return -1;
	}

//	push_data(NULL);
//	show_data(NULL);
//	show_data(NULL);
	pthread_create(&tid1, NULL, push_data, NULL);
	register_thread(tid);
	printf("fist thread created\n");
	pthread_create(&tid2, NULL, show_data, NULL);
	register_thread(tid2);
//	pthread_join(tid1, NULL);
//	pthread_join(tid2, NULL);


}

/*
 * 打开pipe之后，线程的数量和启动先后的问题
 *		  (1)当打开一个pipe之后,pipe的读端和写段一定是要异步的，否则会发生死锁的问题
 *		  (2)当打开一个pipe之后,如果不起线程的情况下,则读写的数据不能超过pipe的buffer的最大值,否则会出现死锁的问题
 *		  (3)当打开一个pipe之后,在超过pipe的buffer最大值的情况下，要启动两个线程，否则也会死锁的问题
 *		  (4)当打开一个pipe之后,在超过pipe的buffer最大值的情况下，如果只想启动一个线程，则先创建一个线程执行读或者写,否则会死锁
 */
