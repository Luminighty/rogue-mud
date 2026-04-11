#ifndef TCP_SERVER_H
#define TCP_SERVER_H


#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

void app_tcp_set_non_blocking(int socket) {
	int flags = fcntl(socket, F_GETFL, 0);
	fcntl(socket, F_SETFL, flags | O_NONBLOCK);
}


int app_tcp_server_listen(int port, int listen_backlog) {
	int server = socket(AF_INET, SOCK_STREAM, 0);
	int opt = 1;
	if (setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		perror("setsockopt(SO_REUSEADDR) failed");
		exit(1);
	}

	app_tcp_set_non_blocking(server);
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(server, (void*)&addr, sizeof(addr)) != 0) {
		perror("Socket init");
		exit(1);
	}

	if (listen(server, listen_backlog) != 0) {
		perror("Listen failed");
		exit(1);
	}
	printf("MUD Server started on port %d\n", port);
	return server;
}


int app_tcp_accept(int socket) {
	return accept(socket, NULL, NULL);
}


void app_tcp_close(int socket) {
	printf("Closing Socket %d\n", socket);
	close(socket);
}

int app_tcp_recv(int socket, void *buffer, size_t length) {
	return recv(socket, buffer, length, 0);
}


int app_tcp_send(int socket, const void *buffer, size_t length) {
	return send(socket, buffer, length, MSG_NOSIGNAL);
}

#endif // TCP_SERVER_H
