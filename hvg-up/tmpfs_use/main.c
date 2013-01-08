#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

int main()
{
	FILE *fp;
	int ret, len;
	char buffer[20] = "hello tmpfs";
	char out_buffer[20] = "\0";
	system("mount -t tmpfs -o size=20m tmpfs /mnt/tmp");
	if ((fp = fopen("/mnt/tmp/test","w+")) == NULL) {
		fprintf(stderr,"ERROR in CreateProcess %s, Process ID %d \n",strerror(errno),getpid());
		return -1;
	}
	len = strlen(buffer);
	printf("-------------\n");
	if ((ret = fwrite(buffer ,1, len, fp)) != len) {
		printf("Len is %d, In fact Len is %d\n", len, ret);
		return -1;
	}
	
	printf("\n-------------\n");

#if 1
	fseek(fp, 0, SEEK_SET);
	fread(out_buffer, 1, len, fp);
	printf("[FILE]: %s\n", out_buffer);
#endif

	return 0;
}
