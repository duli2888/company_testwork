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
	char tmp_name[MAX_NAME];

	struct stat file_state;

	memset(tmp_name, '\0', MAX_NAME);

	if ((fp = fopen("file_list", "r")) == NULL) {
		printf("Opening the file_list file is fail\n");
		return 0;
	}

	while(1) {
		ret = fscanf(fp, "%s",tmp_name);
		if (ret == EOF){
			break;
		}

		if (lstat(tmp_name, &file_state) < 0) 
		{ 
				fprintf(stderr,"error in CreateProcess %s",strerror(errno));
				printf("lstat error for %s\r\n", tmp_name); 
				continue; 
		}

		if (S_ISDIR(file_state.st_mode) != 1){
			if (unlink(tmp_name))
				printf("Delete %s FAILE\n", tmp_name);
			else 
				printf("Delete %s Ok\n",tmp_name);
		}
		memset(tmp_name, '\0', MAX_NAME);
	}

	fseek(fp, 0, SEEK_SET);

	while(1) {
		memset(tmp_name, '\0', MAX_NAME);
		ret = fscanf(fp, "%s",tmp_name);
		if (ret == EOF){
			break;
		}

		if (lstat(tmp_name, &file_state) < 0) 
		{ 
			continue;
		}

		if (S_ISDIR(file_state.st_mode)){
			if (remove(tmp_name))
				printf("Delete %s directory\n", tmp_name);
			else 
				printf("Delete %s Ok\n",tmp_name);
		}
	}

		fclose(fp);
		return 0;
}
