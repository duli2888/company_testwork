#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>




#define BSIZE 200

void str_perm(int perm, char *str_perm);


int main(int argc, char *argv[])
{
	struct dirent *entry;
	struct stat statbuf;
	struct tm *time;
	char timebuff[80], current_dir[BSIZE];
	time_t rawtime;

	char cwd[BSIZE], cwd_orig[BSIZE];                                                                                                                                   

	memset(cwd, 0, BSIZE);
	memset(cwd_orig, 0, BSIZE);

	/* Later we want to go to the original path */
	getcwd(cwd_orig, BSIZE);
	printf("chdir = %s\n", cwd_orig);

	/* Just chdir to specified path */
	if(strlen(argv[1]) > 0 && argv[1][0] != '-') {
		if (chdir(argv[1]) != 0) {
			printf("chdir() error\n");
			return -1;
		}
	}

	getcwd(cwd, BSIZE);
	DIR *dp = opendir(cwd);

	while(entry = readdir(dp)){
		if(stat(entry->d_name, &statbuf)==-1){
			fprintf(stderr, "FTP: Error reading file stats...\n");
		}else{
			char *perms = malloc(9);
			memset(perms,0,9);
			/* Convert time_t to tm struct */
			rawtime = statbuf.st_mtime;
			time = localtime(&rawtime);
			strftime(timebuff, 80, "%b %d %H:%M", time);
			str_perm((statbuf.st_mode & ALLPERMS), perms);
			printf("%c%s %5d %4d %4d %8d %s %s\r\n", 
					(entry->d_type==DT_DIR)?'d':'-',
					perms,statbuf.st_nlink,
					statbuf.st_uid, 
					statbuf.st_gid,
					statbuf.st_size,
					timebuff,
					entry->d_name);
		}
	}
	int ch_ret;
	closedir(dp);
	ch_ret = chdir(cwd_orig);

	int fd;
	char buf[25000];
	if(fork() == 0) {
		if(access(argv[2], R_OK) == 0 && (fd = open(argv[2], O_RDONLY))) {
			read(fd, buf, 25000);
			printf("buf = %s\n", buf);
		}
	}

	return 0;
}


void str_perm(int perm, char *str_perm)
{
	int curperm = 0;
	int flag = 0;
	int read, write, exec;

	/* Flags buffer */
	char fbuff[3];

	read = write = exec = 0;

	int i;
	for(i = 6; i >= 0; i -= 3) {
		/* Explode permissions of user, group, others; starting with users */
		curperm = ((perm & ALLPERMS) >> i ) & 0x7;

		memset(fbuff,0,3);
		/* Check rwx flags for each*/
		read = (curperm >> 2) & 0x1;
		write = (curperm >> 1) & 0x1;
		exec = (curperm >> 0) & 0x1;

		sprintf(fbuff,"%c%c%c",read?'r':'-' ,write?'w':'-', exec?'x':'-');
		strcat(str_perm,fbuff);

	}
}

