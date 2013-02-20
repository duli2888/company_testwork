#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <errno.h>

#define MAX_NAME 256

int main()
{
	FILE *fp;
	int ret;

	if (ret = unlink("aa")) {
	//if (rmdir("aa"))
		printf("Delete %s FAILED: (%d)%s\n", "aa", errno, strerror(errno));
	} else 
		printf("Delete %s Ok\n","aa");

	printf("ret = %d\n", ret );
	return 0;
}
