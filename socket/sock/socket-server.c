#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>

/* Read text from the socket and print it out. Continue until the
 * socket closes. Return nonzero if the client sent a “quit”
 * message, zero otherwise. */
int server(int client_socket) {
	int ret;

	while (1) {
		int length;
		char *text;

		/* First, read the length of the text message from the socket.
		 * read returns zero, the client closed the connection. */
		if (read(client_socket, &length, sizeof(length)) == 0)
			return 0;

		/* Allocate a buffer to hold the text. */
		text = (char *)malloc(length);

		/* Read the text itself, and print it. */
		read(client_socket, text, length);
		printf("%s\n", text);
		ret = !strcmp(text, "quit");

		/* Free the buffer. */
		free(text);

		/* If the client sent the message “quit,” we’re all done. */
		if (ret == 1)
			return 1;
	}
}

int main(int argc, char *const argv[]) {
	const char *const socket_name = argv[1];
	int socket_fd;
	struct sockaddr_un name;
	int client_sent_quit_message;

	/* Create the socket. */
	socket_fd = socket(PF_LOCAL, SOCK_STREAM, 0);
	if (socket_fd == -1) {
		printf("[socket]error: %d\n", errno);
		exit(1);
	}

	/* Indicate that this is a server. */
	name.sun_family = AF_LOCAL;
	strcpy(name.sun_path, socket_name);
	if (bind(socket_fd, (struct sockaddr *)&name, SUN_LEN(&name)) != 0) {
		printf("[bind]error: %d\n", errno);
		exit(1);
	}

	/* Listen for connections. */
	listen(socket_fd, 5);

	/* Repeatedly accept connections, spinning off one server() to deal
	 * with each client. Continue until a client sends a “quit” message */
	do {
		struct sockaddr_un client_name;
		socklen_t client_name_len;
		int client_socket_fd;

		/* Accept a connection. */
		client_name_len = sizeof(client_name);
		client_socket_fd = accept(socket_fd, (struct sockaddr *)&client_name, &client_name_len);
		if (client_socket_fd == -1) {
			printf("[accept]error: %d\n", errno);
			exit(1);
		}

		/* Handle the connection. */
		client_sent_quit_message = server(client_socket_fd);

		/* Close our end of the connection. */
		close(client_socket_fd);
	} while (!client_sent_quit_message);

	/* Remove the socket file. */
	close(socket_fd);
	unlink(socket_name);

	return 0;
}
