#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <fcntl.h>  
#include <limits.h>  
#include <sys/types.h>  
#include <sys/stat.h>  
  
#define FIFO_NAME "my_fifo"  
#define BUFFER_SIZE PIPE_BUF  
  
int main()  
{  
    int pipe_fd;  
    int res;  
	int in_count;
  
    int bytes = 0;  
    char buffer[BUFFER_SIZE + 1];  
  
    if (access(FIFO_NAME, F_OK) == -1)  
    {  
        res = mkfifo(FIFO_NAME, 0777);  
        if (res != 0) { 
            fprintf(stderr, "Could not create fifo %s\n", FIFO_NAME);  
            exit(EXIT_FAILURE);  
        }  
    } 
  
    printf("Process %d opening FIFO O_WRONLY\n", getpid());  
    pipe_fd = open(FIFO_NAME, O_WRONLY); 
    printf("Process %d result %d\n", getpid(), pipe_fd);  

	if (pipe_fd == -1)  
	{  
		exit(EXIT_FAILURE); 
	} 
	while(1) {
		scanf("%s",buffer);
		in_count = bytes = strlen(buffer);	
		fflush(stdin);
		buffer[bytes] = '\n';
		printf("\n[buffer]%s --- Total:%d\n",buffer, in_count);

		while (bytes != 0){
			res = write(pipe_fd, buffer, bytes);  
			if (res == -1){  
				fprintf(stderr, "Write error on pipe\n");  
				exit(EXIT_FAILURE);  
			}  
			lseek(pipe_fd, 0, SEEK_SET);
			bytes = bytes -res;
		}
		printf("This time Has been writed %d bytes\n",in_count);
		bytes = 0;
		in_count = 0;
		res = 0;
	}
	close(pipe_fd);  
	printf("Process %d finish\n", getpid());  
	exit(EXIT_SUCCESS);  
} 
