#include <stdio.h>


int main()
{
	char src[10] = "hello.";
	char dst[15] = "hello,world";
	printf("before memcmp\n");
	if (memcmp(src, dst,5) == 0) {
		printf("src = dst\n");
	}
	printf("after memcmp\n");
	return 0;
}

