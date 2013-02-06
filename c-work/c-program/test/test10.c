#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main()
{
	char *str = (char *)malloc(100);
	strcpy(str, "hello");
	free(str);

	printf("[str = %p]\n", str);
	if (str == NULL) {
		printf("str is NULL\n");
	}

	if(str != NULL){
		strcpy(str, "world");
		printf("[str = %s]\n", str);
	} 

	return 0;
}
