#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <errno.h>

#define MAX_NAME 20

int main()
{
	FILE *fp;
	int ret;
	char tmp_name[MAX_NAME];
	int i = 0,  pre_position = 0;
	int flag = 0;

	struct stat file_state;

	memset(tmp_name, '\0', MAX_NAME);
	
	if ((fp = fopen("file_list", "r")) == NULL) {
		printf("Opening the file_list file is fail\n");
		return 0;
	}


	for (flag = 0; flag < 2; flag ++) {
		while(1) {
			ret = fscanf(fp, "%s",tmp_name);
			if (ret == EOF){
				break;
			}

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
		}
		fseek(fp, 0, SEEK_SET);
	}

	fclose(fp);
	return 0;
}
