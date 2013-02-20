#include <stdio.h>
#include <string.h>

int main()
{
	FILE *fp;
	char line[50];
	char buffer[100] = "VV";
	int cnt;
	fp = fopen("file","r");
	if (fp == NULL)
		return -1;

	while (fgets(line, 4096, fp) != NULL) {
		int cnt;
		cnt = strlen(line);

		while ((line[cnt - 1] == '\r') || (line[cnt - 1] == '\n'))
			--cnt;  

		strncat(buffer, line, cnt + 1);

		printf("buffer[%d] = %d\n",cnt + 2 - 2, buffer[cnt + 2 - 2]);
		printf("buffer[%d] = %d\n",cnt + 2 - 1, buffer[cnt + 2 - 1]);
		printf("buffer[%d] = %d\n",cnt + 2 , buffer[cnt + 2]);
		printf("buffer[%d] = %d\n",cnt + 2 + 1, buffer[cnt + 2 + 1]);
		printf("buffer[%d] = %d\n",cnt + 2 + 2, buffer[cnt + 2 + 2]);
		
		printf("----------------------\n");
	}
	
	return 0;
}
