#include "common.h"

int ftp_server(int para)
{
	char buffer[BSIZE];
	ssize_t bytes_read;
	int connection = para;

	State *state = (State *)malloc(sizeof(State));
	Command *cmd = (Command *)malloc(sizeof(Command));

	char welcome[BSIZE] = "220 ";
	if(strlen(welcome_message) < BSIZE - 4) {
		strcat(welcome, welcome_message);
	} else {
		strcat(welcome, "Welcome to nice FTP service.");
	}

	strcat(welcome, "\n");
	send(connection, welcome, strlen(welcome), 0);						/* Write welcome message */

	while ((bytes_read = recv(connection, (void *)buffer, BSIZE, 0))){				/* Read commands from client */
		if(!(bytes_read > BSIZE)) {
			printf("User %s sent command: %s\n", (state->username==0) ? "unknown" : state->username, buffer); /* TODO: output this to log */
			parse_command(buffer, cmd);
			state->connection = connection;

			/* Ignore non-ascii char. Ignores telnet command */
			//if(buffer[0] <= 127 || buffer[0] >= 0){	// 此加上会使的编译的时候报错
			if (response(cmd, state) == MSQUIT) {
				printf("quit command requst\n");
				break;
			}
			//}
			memset(buffer, 0, BSIZE);
			memset(cmd, 0, sizeof(cmd));
		} else {
			printf("ftp_server:read() error\n"); /* Read error */
		}
	}
	memset(buffer, 0, BSIZE);
	closesocket(connection);
	printf("Client disconnected.\n");

	return 0;
}

/** 
 * Sets up server and handles incoming connections
 * @param port Server port
 */
void server(int port)
{
	int sock = create_socket(port);
	struct sockaddr_in client_address;
	socklen_t len = sizeof(client_address);
	int connection; 

	while(1){			// This can support multiple processes. Just call fork() function, then the child process server for every accept.
		connection = accept(sock, (struct sockaddr *)&client_address,&len);
		ftp_server(connection);
		printf("-----------[another client]---------------------\n");
	}
}

/**
 * Creates socket on specified port and starts listening to this socket
 * @param port Listen on this port
 * @return int File descriptor for new socket
 */
int create_socket(int port)
{
	int sock;
	int reuse = 1;

	/* Server addess */
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;    
	server_address.sin_addr.s_addr = INADDR_ANY;    
	server_address.sin_port = htons(port);

	if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		fprintf(stderr, "Cannot open socket");
		return -1;
	}

	/* Address can be reused instantly after program exits */
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

	/* Bind socket to server address */
	if(bind(sock, (struct sockaddr*)&server_address, sizeof(server_address)) < 0){
		fprintf(stderr, "Cannot bind socket to address");
		return -1;
	}

	listen(sock, 5);
	return sock;
}

/**
 * Accept connection from client
 * @param socket Server listens this
 * @return int File descriptor to accepted connection
 */
int accept_connection(int socket)
{
	socklen_t addrlen = 0;
	struct sockaddr_in client_address;
	addrlen = sizeof(client_address);
	return accept(socket,(struct sockaddr*) &client_address,&addrlen);
}

/**
 * Get ip where client connected to
 * @param sock Commander socket connection
 * @param ip Result ip array (length must be 4 or greater)
 * result IP array e.g. {127,0,0,1}
 */
void getip(int sock, int *ip)
{
	socklen_t addr_size = sizeof(struct sockaddr_in);
	struct sockaddr_in addr;
	getsockname(sock, (struct sockaddr *)&addr, &addr_size);
	int host,i;

	host = (addr.sin_addr.s_addr);
	for(i=0; i<4; i++){
		ip[i] = (host>>i*8)&0xff;
	}
}


char *getpeerip(int sock)
{
	socklen_t addr_size = sizeof(struct sockaddr_in);
	struct sockaddr_in addr;
	printf("before getpeernmae addr_size = %d\n", addr_size);
	if(getpeername(sock, (struct sockaddr *)&addr, &addr_size) != 0) {
		printf("getpeerip:getpeername() error\n");
		return NULL;
	}
	printf("addr point = %08x addr_size = %d\n", addr.sin_addr.s_addr, addr_size);
	return inet_ntoa(addr.sin_addr);
}


/**
 * Lookup enum value of string
 * @param cmd Command string 
 * @return Enum index if command found otherwise -1
 */
int lookup_cmd(char *cmd){
	const int cmdlist_count = sizeof(cmdlist_str) / sizeof(char *);
	return lookup(cmd, cmdlist_str, cmdlist_count);
}

/**
 * General lookup for string arrays
 * It is suitable for smaller arrays, for bigger ones trie is better
 * data structure for instance.
 * @param needle String to lookup
 * @param haystack Strign array
 * @param count Size of haystack
 */
int lookup(char *needle, const char **haystack, int count)
{
	int i;
	for(i = 0; i < count; i++){
		if(strcmp(needle, haystack[i]) == 0)
			return i;
	}
	return -1;
}


/** 
 * Writes current state to client
 */
void write_state(State *state)
{
	send(state->connection, state->message, strlen(state->message), 0);
}

/**
 * Generate random port for passive mode
 * @param state Client state
 */
void gen_port(Port *port)
{
	srand(time(NULL));
	port->p1 = 128 + (rand() % 64);
	port->p2 = rand() % 0xff;

}

/**
 * Parses FTP command string into struct
 * @param cmdstring Command string (from ftp client)
 * @param cmd Command struct
 */
void parse_command(char *cmdstring, Command *cmd)
{
	sscanf(cmdstring,"%s %s",cmd->command,cmd->arg);
}

int ftp_server(void)
{
	server(21);
	return 0;
}

int create_connecte(State *state, int port)
{
	// 创建用于internet的流协议(TCP)socket,用client_socket代表客户机socket
	int client_socket = socket(AF_INET, SOCK_STREAM, 0); 
	if( client_socket < 0) {
		printf("Create Socket Failed!\n");
		return -1; 
	}   
	char peerip[16];// = "172.21.123.28";
	socklen_t addr_size = sizeof(struct sockaddr_in);
	struct sockaddr_in addr;
	if(getpeername(state->connection, (struct sockaddr *)&addr, &addr_size) != 0) {
		printf("getpeerip:getpeername() error\n");
		return -1;
	}
	memcpy(peerip, inet_ntoa(addr.sin_addr), 16);

#if 1
	struct sockaddr_in client_addr;
	bzero(&client_addr, sizeof(client_addr));           // 把一段内存区的内容全部设置为0
	client_addr.sin_family = AF_INET;                   // internet协议族
	client_addr.sin_addr.s_addr = htons(INADDR_ANY);  // INADDR_ANY表示自动获取本机地址
	//client_addr.sin_addr.s_addr = inet_addr("172.21.123.27");   // INADDR_ANY表示自动获取本机地址
	client_addr.sin_port = htons(20);                   // 0表示让系统自动分配一个空闲端口, 这里分配了一个ftp-data端口
#if 1
	int reuse = 1;
	if(setsockopt(client_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) != 0) {
		printf("create_connecte:setsockopt() error\n");
		return -1;
	}
#endif
	// 把客户机的socket和客户机的socket地址结构联系起来
	if( bind(client_socket, (struct sockaddr*)&client_addr, sizeof(client_addr))) {
		printf("Client Bind Port Failed!\n");
		return -1; 
	}   
#endif

	// 设置一个socket地址结构server_addr,代表服务器的internet地址, 端口
	struct sockaddr_in server_addr;
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	if(inet_aton(peerip, &server_addr.sin_addr) == 0) {
		printf("Server IP Address Error!\n");
		return -1; 
	}   
	server_addr.sin_port = htons(port);
	socklen_t server_addr_length = sizeof(server_addr);

	// 向服务器发起连接,连接成功后client_socket代表了客户机和服务器的一个socket连接         
	//printf("[before connect]fd = %d, addrlen = %d, addr.sin_len = %d, addr.sin_family = %d, addr.sin_port = %d, addr.sin_addr = %d\n", client_socket, server_addr_length, server_addr.sin_len, server_addr.sin_family, server_addr.sin_port, server_addr.sin_addr);
	if(connect(client_socket, (struct sockaddr *)&server_addr, server_addr_length) < 0) {
		printf("Can Not Connect To %s!\n", peerip);
		return -1;
	}
	//printf("[after connect]fd = %d, addrlen = %d, addr.sin_len = %d, addr.sin_family = %d, addr.sin_port = %d, addr.sin_addr = %d\n", client_socket, server_addr_length, server_addr.sin_len, server_addr.sin_family, server_addr.sin_port, server_addr.sin_addr);

	return client_socket;
}

