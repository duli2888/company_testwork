#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void show_handler(int sig)
{
	printf("I got signal %d\n", sig);
	int i;
	for(i = 0; i < 5; i++) {
		printf("i = %d\n", i);
		sleep(1);
	}
}

int main(void)
{
	int i = 0;
	struct sigaction act, oldact;
	act.sa_handler = show_handler;
	sigaddset(&act.sa_mask, SIGQUIT);			//见注(1)
	act.sa_flags = SA_RESETHAND | SA_NODEFER;	//见注(2)
	//act.sa_flags = 0;							//见注(3)

	sigaction(SIGINT, &act, &oldact);
	while(1) {
		sleep(1);
		printf("sleeping %d\n", i);
		i++;
	}

	return 0;
}


/*
 *	注：
 *	(1) 如果在信号SIGINT(Ctrl + c)的信号处理函数show_handler执行过程中，本进程收到信号SIGQUIT(Crt+\)，将阻塞该信号，直到show_handler执行结束才会处理信号SIGQUIT。
 *	(2) SA_NODEFER		一般情况下，当信号处理函数运行时，内核将阻塞<该给定信号 -- SIGINT>。但是如果设置了SA_NODEFER标记，那么在该信号处理函数运行时，内核将不会阻塞该信号。
 *						SA_NODEFER是这个标记的正式的POSIX名字(还有一个名字SA_NOMASK，为了软件的可移植性，一般不用这个名字)
 *		SA_RESETHAND	当调用信号处理函数时，将信号的处理函数重置为缺省值。SA_RESETHAND是这个标记的正式的POSIX名字(还有个名字SA_ONESHOT，为了软件的可移植性，
 *						一般不用这个名字)
 *	(3) 如果不需要重置该给定信号的处理函数为缺省值；并且不需要阻塞该给定信号(无须设置sa_flags标志)，那么必须将sa_flags清零，否则运行将会产生段错误。
 *		但是sa_flags清零后可能会造成信号丢失！
 *
 *  使用:
 *			可以将(2)和(3)分别使用，执行程序的时候，则连续Ctrl + c 多次，则可以看出区别来。打开(2), 多次Ctrl + c，则会除了接收到第一个信号之外，会同时处理第二个信号，
 *		因为有SA_RESETHAND标志，则会使得信号处理函数重置为缺省值，致使程序退出。所以注册的信号处理程序只是调用一次，一次之后使用缺省值。
 *		如果打开(3), 多次Ctrl + c， 则会接收到第一个信号之外，对于第二个信号则阻塞，等到第一个信号处理程序结束后才能执行第二个信号处理程序。
 *
 */
