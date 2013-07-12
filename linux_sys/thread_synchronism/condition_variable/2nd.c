#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

struct node {
	int n_number;
	struct node *n_next;
};

struct node *head = NULL;

static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

/*[thread_func]*/
static void cleanup_handler1(void *arg)
{
    printf("Cleanup thread----[1].\n");
    free(arg);
    (void)pthread_mutex_unlock(&mtx);
}

static void cleanup_handler2(void *arg)
{
    printf("Cleanup thread----[2].\n");
    free(arg);
    (void)pthread_mutex_unlock(&mtx);
}

static void *thread1(void *arg)
{
    struct node *p = NULL;

    pthread_cleanup_push(cleanup_handler1, p);

	while (1) {
		pthread_mutex_lock(&mtx);					// 这个mutex主要是用来保证pthread_cond_wait的并发性
		/*
		 * 这个while要特别说明一下，单个pthread_cond_wait功能很完善，为何这里要有一个while (head == NULL)呢？
		 * 因为pthread_cond_wait里的线程可能会被意外唤醒，如果这个时候head == NULL，则不是我们想要的情况。这个时候，应该让线程继续进入pthread_cond_wait
		 */
		while (head == NULL)   {
			pthread_cond_wait(&cond, &mtx);         
			// 用这个流程是比较清楚的 block-->unlock-->wait() return-->lock
		}
		p = head;
		head = head->n_next;						// 链表头部删除节点
		printf("THREAD[1]: Got %d from front of queue.\n", p->n_number);
		free(p);
		pthread_mutex_unlock(&mtx);					// 临界区数据操作完毕，释放互斥锁
	}
	pthread_cleanup_pop(0);
	return 0;
}

static void *thread2(void *arg)
{
    struct node *p = NULL;

    pthread_cleanup_push(cleanup_handler2, p);

	while (1) {
		pthread_mutex_lock(&mtx);					// 这个mutex主要是用来保证pthread_cond_wait的并发性
		while (head == NULL)   {
			pthread_cond_wait(&cond, &mtx);         
		}
		p = head;
		head = head->n_next;						// 链表头部删除节点
		printf("THREAD[2]: Got %d from front of queue.\n", p->n_number);
		free(p);
		pthread_mutex_unlock(&mtx);					// 临界区数据操作完毕，释放互斥锁
	}

	pthread_cleanup_pop(0);
	return 0;
}

int main(void)
{
    pthread_t tid1, tid2;
    int i;
    struct node *p;

    pthread_create(&tid1, NULL, thread1, NULL);
    pthread_create(&tid2, NULL, thread2, NULL);
	
    /*[tx6-main]*/
    for (i = 0; i < 15; i++) {
        p = malloc(sizeof(struct node));
        p->n_number = i;

        pthread_mutex_lock(&mtx);					// 需要操作head这个临界资源，先加锁
        p->n_next = head;							// 链表头部插入节点
        head = p;
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mtx);

        sleep(1);
    }

    printf("Parent thread wanna end the line. So cancel Child thread.\n");
	/*
	 * 关于pthread_cancel，有一点额外的说明，它是从外部终止子线程，子线程会在最近的取消点，退出线程，而在我们的代码里，
	 * 最近的取消点肯定就是pthread_cond_wait()了。关于取消点的信息，有兴趣可以google,这里不多说了
	 */
    pthread_cancel(tid1);
    pthread_cancel(tid2);
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    printf("All done -- exiting.\n");
    return 0;
}
