#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>

void tv_sub(struct timeval *out, struct timeval *in);

int main()
{
	int rv, i;
	struct timeval tv1, tv2;
	unsigned int ms, um;

	if (gettimeofday(&tv1, NULL) < 0) {
		printf("ERROR: gettimeofday()\n");
		return -1;	
	}

	//rv = rand();
	rv = 5000000;
	for (i = 0; i < rv; i++)
		;

	printf("%d\n", rv);
	if (gettimeofday(&tv2, NULL) < 0) {
		printf("ERROR: gettimeofday()\n");
		return -1;	
	}
	tv_sub(&tv2, &tv1);
	ms = tv2.tv_usec / 1000 + tv2.tv_sec * 1000;          /* 毫秒的整数部分 */
	um = tv2.tv_usec % 1000;                                 /* 毫秒的小数部分 */

	printf("秒%d + 微秒%d\n", (unsigned int)tv2.tv_sec, (unsigned int)tv2.tv_usec);
	printf("毫秒%d + %d\n", ((unsigned int)tv2.tv_sec) * 1000, ((unsigned int)tv2.tv_usec) / 1000);
	printf("共耗时 %d.%03dms\n", ms, um);

	return 0;
}

/* 两个timeval结构相减 */
void tv_sub(struct timeval *out, struct timeval *in)
{
	if((out->tv_usec -= in->tv_usec) < 0) {		// 先减低位的微妙，如果不够，向高位借1
		-- out->tv_sec;
		out->tv_usec += 1000000;
	}   
	out->tv_sec -= in->tv_sec;					// 高位秒的差值
}

