#include "common.h"

int g_port;

/**
 * Generates response message for client
 * @param cmd Current command
 * @param state Current connection state
 */
int response(Command *cmd, State *state)
{
	switch(lookup_cmd(cmd->command)){
		case USER: ftp_user(cmd,state); break;
		case PASS: ftp_pass(cmd,state); break;
		case PASV: ftp_pasv(cmd,state); break;
		case LIST: ftp_list(cmd,state); break;
		//case CWD:  ftp_cwd(cmd,state); break;
		case PWD:  ftp_pwd(cmd,state); break;
		//case MKD:  ftp_mkd(cmd,state); break;
		//case RMD:  ftp_rmd(cmd,state); break;
		case RETR: ftp_retr(cmd,state); break; // ftp client: get command
		case STOR: ftp_stor(cmd,state); break;
		//case DELE: ftp_dele(cmd,state); break;
		//case SIZE: ftp_size(cmd,state); break;
		//case ABOR: ftp_abor(state); break;
		case QUIT: ftp_quit(state); return MSQUIT;
		//case TYPE: ftp_type(cmd,state); break;
		case PORT: ftp_port(cmd, state); break;
		case NOOP:
				   if(state->logged_in){
					   state->message = (char *)"200 Nice to NOOP you!\n";
				   }else{
					   state->message = (char *)"530 NOOB hehe.\n";
				   }
				   write_state(state);
				   break;
		default: 
				   state->message = (char *)"500 Unknown command\n";
				   write_state(state);
				   break;
	}

	return 0;
}

/**
 * Handle USER command
 * @param cmd Command with args
 * @param state Current client connection state
 */
void ftp_user(Command *cmd, State *state)
{
	printf("call ftp_user\n");
	const int total_usernames = sizeof(usernames) / sizeof(char *);
	if (lookup(cmd->arg, usernames, total_usernames) >= 0) {
		state->username = (char *)malloc(32);
		memset(state->username, 0, 32);
		strcpy(state->username, cmd->arg);
		state->username_ok = 1;
		state->message = (char *)"331 User name okay, need password\n";
	} else {
		state->message = (char *)"530 Invalid username\n";
	}
	write_state(state);
}

/** PASS command */
void ftp_pass(Command *cmd, State *state)
{
	(void)cmd;
	if (state->username_ok == 1) {
		state->logged_in = 1;
		state->message = (char *)"230 Login successful\n";
	} else {
		state->message = (char *)"500 Invalid username or password\n";
	}
	write_state(state);
}

/** PASV command */
void ftp_pasv(Command *cmd, State *state)
{
	(void)cmd;
	if(state->logged_in){
		int ip[4];
		char buff[255];
		char *response = (char *)"227 Entering Passive Mode (%d,%d,%d,%d,%d,%d)\n";
		Port *port = (Port *)malloc(sizeof(Port));
		gen_port(port);
		getip(state->connection,ip);

		/* Close previous passive socket? */
		closesocket(state->sock_pasv);

		/* Start listening here, but don't accept the connection */
		state->sock_pasv = create_socket((256*port->p1)+port->p2);
		printf("port: %d\n",256*port->p1+port->p2);
		sprintf(buff,response,ip[0],ip[1],ip[2],ip[3],port->p1,port->p2);
		state->message = buff;
		state->mode = SERVER;
		puts(state->message);

	}else{
		state->message = (char *)"530 Please login with USER and PASS.\n";
		printf("%s",state->message);
	}
	write_state(state);
}

/** LIST command */
int ftp_list(Command *cmd, State *state)
{
	if(state->logged_in == 1){
		int connection;
		/* TODO: dynamic buffering maybe? */
		char cwd[BSIZE], cwd_orig[BSIZE];
		memset(cwd, 0, BSIZE);
		memset(cwd_orig, 0, BSIZE);

		/* Just chdir to specified path */
		if(strlen(cmd->arg) > 0 && cmd->arg[0] != '-') {
		}

		if(0) {
			state->message = (char *)"550 Failed to open directory.\n";
		} else {
			if(state->mode == SERVER) {
				connection = create_connecte(state, g_port);

				state->message = (char *)"150 Here comes the directory listing.\n";
				write_state(state);
				//puts(state->message);

				if(0){
					printf("FTP: Error reading file stats...\n");
				}else{
					int sent_total = -1;
					char sbuf[100];
					strcpy(sbuf, "lrwxrwxrwx   1 root root    29  Dec 13 09:50 hello.txt\r\n");
					sent_total = send(connection, sbuf, strlen(sbuf), 0); 
					//printf("send sbuf over: sent_total = %d\n", sent_total);
					if (sent_total > 0) {
						state->message = (char *)"226 Transfer complete.\n";
					} else {
						state->message = (char *)"550 Failed to read file.\n";
					}

				}
				closesocket(connection);
				state->message = (char *)"226 Directory send OK.\n";
				write_state(state);
				state->mode = NORMAL;
				//close(state->sock_pasv);
				//write_state(state);
			}else if(state->mode == CLIENT){
				state->message = (char *)"502 Command not implemented.\n";
			}else{
				state->message = (char *)"425 Use PASV or PORT first.\n";
			}
		}
	}else{
		state->message = (char *)"530 Please login with USER and PASS.\n";
	}
	state->mode = NORMAL;
	return 0;
}



/** QUIT command */
void ftp_quit(State *state)
{
	state->message = (char *)"221 Goodbye, friend. MicroKernl\n";
	write_state(state);
	closesocket(state->connection);
}

/** PWD command */
void ftp_pwd(Command *cmd, State *state)
{
	(void)cmd;
	if(state->logged_in){
		char cwd[BSIZE];
		char result[BSIZE];
		memset(result, 0, BSIZE);
		if(getcwd(cwd,BSIZE)!=NULL){
			strcat(result,"257 \"");
			strcat(result,cwd);
			strcat(result,"\"\n");
			state->message = result;
		}else{
			state->message = (char *)"550 Failed to get pwd.\n";
		}
		write_state(state);
	}
}

/** CWD command */
void ftp_cwd(Command *cmd, State *state)
{
	if(state->logged_in){
		if(chdir(cmd->arg)==0){
			state->message = (char *)"250 Directory successfully changed.\n";
		}else{
			state->message = (char *)"550 Failed to change directory.\n";
		}
	}else{
		state->message = (char *)"500 Login with USER and PASS.\n";
	}
	write_state(state);

}

#if 1
/** 
 * MKD command 
 * TODO: full path directory creation
 */
void ftp_mkd(Command *cmd, State *state)
{
	if(state->logged_in){
		char cwd[BSIZE];
		char res[BSIZE];
		memset(cwd,0,BSIZE);
		memset(res,0,BSIZE);
		getcwd(cwd,BSIZE);

		/* TODO: check if directory already exists with chdir? */

		/* Absolute path */
		if(cmd->arg[0]=='/'){
			if(mkdir(cmd->arg,S_IRWXU)==0){
				strcat(res,"257 \"");
				strcat(res,cmd->arg);
				strcat(res,"\" new directory created.\n");
				state->message = res;
			}else{
				state->message = (char *)"550 Failed to create directory. Check path or permissions.\n";
			}
		} else { /* Relative path */
			if(mkdir(cmd->arg,S_IRWXU)==0){
				sprintf(res,"257 \"%s/%s\" new directory created.\n",cwd,cmd->arg);
				state->message = res;
			}else{
				state->message = (char *)"550 Failed to create directory.\n";
			}
		}
	}else{
		state->message = (char *)"500 Good news, everyone! There's a report on TV with some very bad news!\n";
	}
	write_state(state);
}
#endif


/** Handle STOR command. TODO: check permissions. */
void ftp_stor(Command *cmd, State *state)
{
	(void)cmd;
	int connection; 

	if(state->logged_in) {
		if(!(state->mode == SERVER)) {
			state->message = (char *)"550 Please use PASV instead of PORT.\n";
		} else { /* Passive mode */
			connection = create_connecte(state, g_port);
			//state->message = "125 Data connection already open; transfer starting.\n";
			state->message = (char *)"150 Data connection already open; transfer starting.\n";
			write_state(state);

			/* Using splice function for file receiving.
			 * The splice() system call first appeared in Linux 2.6.17.
			 */
			printf("----------------------------------sbuf-------------------------------\n");
			if(0){
				printf("FTP: Error reading file stats...\n");
			} else {
				int recv_total = -1;
				char sbuf[1500];
				int i = 0;;
				while (1) {
					recv_total = recv(connection, sbuf, 1500, 0);
					printf("\n================[%d] number of times, recv_total = %d==========================\n",i, recv_total);
					if (recv_total > 0) {	// 接收到文件
						printf("%s", sbuf);
					} else if (recv_total == 0) {
						state->message = (char *)"226 File send OK.\n";
						goto result;
					}
					else {		// 接收文件错误
						state->message = (char *)"550 Failed to read file.\n";
						goto result;
					}
					memset(sbuf, 0, 1500);
					++i;
				}
			}
result:
			printf("----------------------------------------------------------------------\n");
			closesocket(connection);
			write_state(state);
			/* TODO: signal with ABOR command to exit */
		}
	}else{
		state->message = (char *)"530 Please login with USER and PASS.\n";
	}
	state->mode = NORMAL;
}

int ftp_retr(Command *cmd, State *state)
{
	int connection = -1, sent_total = 0;
	char file_name[BSIZE];
	if(state->logged_in) {
		if(state->mode == SERVER){
			char cwd_orig[BSIZE];
			getcwd(cwd_orig, BSIZE);
			strcpy(file_name, cmd->arg);
			if(strcmp(file_name, "hello.txt") == 0) {
				state->message = (char *)"150 Opening BINARY mode data connection.\n";
				write_state(state);
				connection = create_connecte(state, g_port);
				//close(state->sock_pasv);

				char sbuf[100] = "hello, This is microkernel ftp server data.";
				sent_total = send(connection, sbuf, strlen(sbuf), 0); 
				//printf("send sbuf over: sent_total = %d\n", sent_total);
				if (sent_total > 0) {
					state->message = (char *)"226 Transfer complete.\n";
				} else {
					state->message = (char *)"550 Failed to read file.\n";
				}   
			} else {
				printf("open %s error\n", cmd->arg);
				state->message = (char *)"550 Failed to get file\n";
			}   
		} else {
			state->message = (char *)"550 Please use PASV instead of PORT.\n";
		}   
	} else {
		state->message = (char *)"530 Please login with USER and PASS.\n";
	}   
	//close(connection);
	closesocket(connection);
	write_state(state);

	printf("call ftp_retr\n");
	state->mode = NORMAL;
	//close(state->sock_pasv);
	return 0;
}


/** ABOR command */
void ftp_abor(State *state)
{
	if(state->logged_in){
		state->message = (char *)"226 Closing data connection.\n";
		state->message = (char *)"225 Data connection open; no transfer in progress.\n";
	}else{
		state->message = (char *)"530 Please login with USER and PASS.\n";
	}
	write_state(state);

}

/** 
 * Handle TYPE command.
 * BINARY only at the moment.
 */
void ftp_type(Command *cmd,State *state)
{
	if(state->logged_in){
		if(cmd->arg[0]=='I'){
			state->message = (char *)"200 Switching to Binary mode.\n";
		}else if(cmd->arg[0]=='A'){

			/* Type A must be always accepted according to RFC */
			state->message = (char *)"200 Switching to ASCII mode.\n";
		}else{
			state->message = (char *)"504 Command not implemented for that parameter.\n";
		}
	}else{
		state->message = (char *)"530 Please login with USER and PASS.\n";
	}
	write_state(state);
}

/** Handle DELE command */
void ftp_dele(Command *cmd,State *state)
{
	if(state->logged_in){
		if(unlink(cmd->arg)==-1){
			state->message = (char *)"550 File unavailable.\n";
		}else{
			state->message = (char *)"250 Requested file action okay, completed.\n";
		}
	}else{
		state->message = (char *)"530 Please login with USER and PASS.\n";
	}
	write_state(state);
}

/** Handle RMD */
void ftp_rmd(Command *cmd, State *state)
{
	if(!state->logged_in){
		state->message = (char *)"530 Please login first.\n";
	}else{
		if(rmdir(cmd->arg)==0){
			state->message = (char *)"250 Requested file action okay, completed.\n";
		}else{
			state->message = (char *)"550 Cannot delete directory.\n";
		}
	}
	write_state(state);

}

/** Handle SIZE (RFC 3659) */
void ftp_size(Command *cmd, State *state)
{
	if(state->logged_in){
		struct stat statbuf;
		char filesize[128];
		memset(filesize,0,128);
		/* Success */
		if(stat(cmd->arg,&statbuf)==0){
			sprintf(filesize, "213 %d\n", (int)statbuf.st_size);
			state->message = filesize;
		}else{
			state->message = (char *)"550 Could not get file size.\n";
		}
	}else{
		state->message = (char *)"530 Please login with USER and PASS.\n";
	}

	write_state(state);

}

/** 
 * Converts permissions to string. e.g. rwxrwxrwx 
 * @param perm Permissions mask
 * @param str_perm Pointer to string representation of permissions
 */
void str_perm(int perm, char *str_perm)
{
	int curperm = 0;
	// int flag = 0;
	int read, write, exec;

	/* Flags buffer */
	char fbuff[3];

	read = write = exec = 0;

	int i;
	for(i = 6; i>=0; i-=3){
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

void ftp_port(Command *cmd, State *state)
{
	char port1[4], port2[4];
	int lport = 0;
	int i = 0, n = 0;
	if(state->logged_in) {
		while (n != 4) {
			if (cmd->arg[i] == ',') n++;
			i++;
		}   

		port1[0] = cmd->arg[i];
		port1[1] = cmd->arg[i + 1]; 
		port1[2] = cmd->arg[i + 2]; 
		port1[3] = '\0';

		port2[0] = cmd->arg[i + 4]; 
		port2[1] = cmd->arg[i + 5]; 
		port2[2] = cmd->arg[i + 6]; 
		port2[3] = '\0';

		// 得到client要求server监听的端口, server端的端口号是20, 监听的port端口号
		//printf("atoi(port1) = %d atoi(port2) = %d\n", atoi(port1), atoi(port2));
		lport = atoi(port1) * 256 + atoi(port2);
		state->mode = SERVER;
		state->message = (char *)"200 PORT command successful.\n";
		g_port = lport;
		//printf("ftp_port : port number = %d\n", lport);
	} else {
		state->message = (char *)"530 Please login with USER and PASS.\n";
	}
	write_state(state);
}
