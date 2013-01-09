#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <errno.h>

#define MAX_FILE_LENTH 4096
#define MAX_NAME 20

int main()
{
	FILE *fp;
	int ret;
	char buffer[MAX_FILE_LENTH];
	char tmp_name[MAX_NAME];
	int i = 0, letter_count = 0, pre_position = 0;
	int flag = 0;

	struct stat file_state;

	memset(buffer, '\0', MAX_FILE_LENTH);
	memset(tmp_name, '\0', MAX_NAME);
	
	if ((fp = fopen("file_list", "r")) == NULL) {
		printf("Opening the file_list file is fail\n");
		return 0;
	}

	fread(buffer, 1, MAX_FILE_LENTH, fp);
	int total_len = strlen(buffer);
	pre_position = 0;

	for (flag = 0; flag < 2; flag ++) {
		do {
			while(buffer[i] != '\n') {
				i++;
			}
			letter_count = i - pre_position;
			strncpy(tmp_name, &buffer[pre_position], letter_count);
			if (lstat(tmp_name, &file_state) < 0) 
			{ 
				if (flag == 1) {
					goto end;
				} else {
					fprintf(stderr,"error in CreateProcess %s",strerror(errno));
					printf("lstat error for %s\r\n", tmp_name); 
					return -1; 
				}
			}

			if (flag == 1) {  // file delete a derctory
				if (S_ISDIR(file_state.st_mode)){
					if (remove(tmp_name))
						printf("Delete %s FAILE\n", tmp_name);
					else 
						printf("Delete %s Ok\n",tmp_name);
				}
			} else if(flag == 0) {  // delete a general file
				if (S_ISREG(file_state.st_mode)){
					if (remove(tmp_name))
						printf("Delete %s FAILE\n", tmp_name);
					else 
						printf("Delete %s Ok\n",tmp_name);
				}
			}
end:
			memset(tmp_name, '\0', MAX_NAME);
			i++;
			letter_count++;
			pre_position = i;
		}while(i < total_len);
		pre_position = 0;
		i = 0; letter_count = 0; pre_position = 0;
	}
	fclose(fp);
	return 0;
}
