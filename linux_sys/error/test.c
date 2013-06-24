#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int main(int argc,char *argv[])
{
	FILE *fp;
	char *buf;

	if((fp = fopen(argv[1], "r")) == NULL) {
		perror("perror");
		printf("strerror : [%d]%s\n", errno, strerror(errno));
		exit(1);
	}
	perror("perror");
	errno = 13;
	printf("strerror : [%d]%s\n", errno, strerror(errno));
	fclose(fp);

	return 0;
}

