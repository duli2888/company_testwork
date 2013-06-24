#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pwd.h>
//#include <netinet/in.h>
#include <time.h>
#include <dirent.h>
#include <errno.h>
#include "../sockets.h"

#ifndef BSIZE
#define BSIZE 1024
#endif

#define MSQUIT	100

typedef struct Port {
	int p1;
	int p2;
} Port;

typedef struct State {
	int mode;				/* Connection mode: NORMAL, SERVER, CLIENT */

	int logged_in;			/* Is user loggd in? */

	int username_ok;		/* Is this username allowed? */
	char *username;

	char *message;			/* Response message to client e.g. 220 Welcome */

	int connection;			/* Commander connection */

	int sock_pasv;			/* Socket for passive connection (must be accepted later) */

	int tr_pid;				/* Transfer process id */
} State;


/* Command struct */
typedef struct Command {
  char command[5];
  char arg[BSIZE];
} Command;

/**
 * Connection mode
 * NORMAL - normal connection mode, nothing to transfer
 * SERVER - passive connection (PASV command), server listens
 * CLIENT - server connects to client (PORT command)
 */
typedef enum conn_mode{ NORMAL, SERVER, CLIENT }conn_mode;

/* Commands enumeration */
typedef enum cmdlist { 
  ABOR, CWD, DELE, LIST, MDTM, MKD, NLST, PASS, PASV,
  PORT, PWD, QUIT, RETR, RMD, RNFR, RNTO, SITE, SIZE,
  STOR, TYPE, USER, NOOP
} cmdlist;

/* String mappings for cmdlist */
static const char *cmdlist_str[] = {
  "ABOR", "CWD", "DELE", "LIST", "MDTM", "MKD", "NLST", "PASS", "PASV",
  "PORT", "PWD", "QUIT", "RETR", "RMD", "RNFR", "RNTO", "SITE", "SIZE",
  "STOR", "TYPE", "USER", "NOOP" 
};

/* Valid usernames for anonymous ftp */
static const char *usernames[] = {
  "ftp","anonymous","public","anon","test","foo","siim"
};

/* Welcome message */
static char welcome_message[] = "Welcome to the FTP server of microkernel!";

/* Server functions */
void gen_port(Port *);
void parse_command(char *, Command *);
int create_socket(int port);
void write_state(State *);
int accept_connection(int);

/* Commands handle functions*/
int response(Command *, State *);
void ftp_user(Command *, State *);
void ftp_pass(Command *, State *);
void ftp_pwd(Command *, State *);
void ftp_cwd(Command *, State *);
void ftp_mkd(Command *, State *);
void ftp_rmd(Command *, State *);
void ftp_pasv(Command *, State *);
int ftp_list(Command *, State *);
int ftp_retr(Command *, State *);
void ftp_stor(Command *, State *);
void ftp_dele(Command *, State *);
void ftp_size(Command *, State *);
void ftp_quit(State *);
void ftp_type(Command *, State *);
void ftp_abor(State *);
void ftp_port(Command *cmd, State *state);

void str_perm(int, char *);
void my_wait(int);

///////////////////////////////////////////////////////////////
int lookup(char *needle, const char **haystack, int count);
int lookup_cmd(char *cmd);
void getip(int sock, int *ip);
int create_connecte(State *, int port);
