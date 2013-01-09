#include <stdio.h>
#include <string.h>

#define MAX_FILE_LENTH 4096
#define MAX_NAME 20

int main()
{
	FILE *fp;
	int ret;
	char buffer[MAX_FILE_LENTH];
	char tmp_name[MAX_NAME];
	int i = 0, letter_count = 0, pre_position = 0;

	memset(buffer, '\0', MAX_FILE_LENTH);
	if ((fp = fopen("file_list", "r")) == NULL) {
		printf("Opening the file_list file is fail\n");
		return 0;
	}

	fread(buffer, 1, MAX_FILE_LENTH, fp);
	int total_len = strlen(buffer);
	pre_position = 0;

	do {
		while(buffer[i] != '\n') {
			i++;
		}
		letter_count = i - pre_position;
		strncpy(tmp_name, &buffer[pre_position], letter_count);
		if (remove(tmp_name))
			printf("Delete %s faile\n", tmp_name);
		else 
			printf("Delete %s Ok\n",tmp_name);
		i++;
		letter_count++;
		pre_position = i;
	}while(i < total_len);

	fclose(fp);
	return 0;
}
