#include "network.h"
#include "client.h"
#include "config.h"
#include "game.h"
#include "key.h"

#include <asm-generic/errno-base.h>
#include <asm-generic/errno.h>
#include <asm-generic/socket.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>


static Server server = {0};

static void set_non_blocking(int fd) {
	int flags = fcntl(fd, F_GETFL, 0);
	fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

void network_init() {
	server.socket = socket(AF_INET, SOCK_STREAM, 0);

	int opt = 1;
	if (setsockopt(server.socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
	    perror("setsockopt(SO_REUSEADDR) failed");
		exit(1);
	}

	set_non_blocking(server.socket);
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(NETWORK_PORT);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(server.socket, (void*)&addr, sizeof(addr)) != 0) {
		perror("Socket init");
		exit(1);
	}

	if (listen(server.socket, 128) != 0) {
		perror("Listen failed");
		exit(1);
	}
	printf("MUD Server started on port %d\n", NETWORK_PORT);
}

void network_destroy() {
	close(server.socket);
	for (int i = 0; i < PLAYER_COUNT; i++) {
		if (server.clients[i].connected)
			close(server.clients[i].socket);
	}
}

static inline int init_fd_sets(fd_set *read_fds, fd_set *write_fds) {
	FD_ZERO(read_fds);
	FD_ZERO(write_fds);
	FD_SET(server.socket, read_fds);
	int max_fd = server.socket;
	for (int i = 0; i < PLAYER_COUNT; i++) {
		if (!server.clients[i].connected)
			continue;
		FD_SET(server.clients[i].socket, read_fds);
		if (server.clients[i].socket > max_fd)
			max_fd = server.clients[i].socket;
	}
	return max_fd;
}


static inline Client *push_client() {
	for (int i = 0; i < PLAYER_COUNT; i++) {
		if (!server.clients[i].connected) {
			memset(&server.clients[i], 0, sizeof(Client));
			server.clients[i].connected = true;
			server.clients[i].player_index = i;
			server.clients[i].player_index = i;
			return &server.clients[i];
		}
	}
	assert(0);
}

#define ANSI_CURSOR_HIDE "\033[?25l"
#define ANSI_CLEAR "\x1b[2J"
static const char accept_message[] = ANSI_CURSOR_HIDE ANSI_CLEAR;
static const unsigned char telnet_mode[] = {
	255, 251, 1, 
	255, 251, 3, 
	255, 254, 34
};

static inline void accept_clients(fd_set *read_fds) {
	if (!FD_ISSET(server.socket, read_fds))
		return;

	while (1) {
		int socket = accept(server.socket, NULL, NULL);
		if (socket < 0) {
			if (errno != EWOULDBLOCK && errno != EAGAIN)
				perror("Accept error");
			return;
		}
		Client *client = push_client();
		client->socket = socket;
		set_non_blocking(socket);

		send(socket, telnet_mode, sizeof(telnet_mode), 0);
		send(socket, accept_message, sizeof(accept_message), 0);
	}
}


static void handle_client_escaped_char(Client *client, char c) {
	switch (c) {
	case 'A':
		game_input(client->player_index, KEY_UP);
		break;
	case 'B':
		game_input(client->player_index, KEY_DOWN);
		break;
	case 'C':
		game_input(client->player_index, KEY_RIGHT);
		break;
	case 'D':
		game_input(client->player_index, KEY_LEFT);
		break;
	default: // UNKNOWN
		break;
	}
}
static void handle_client_char(Client *client, char c) {
	game_input(client->player_index, c);
}

static void handle_client_message(Client *client) {
	static const char ESCAPE_CHAR = 27;
	int i = 0;
	while(i < client->in_len) {
		if (client->in_buffer[i] == ESCAPE_CHAR) {
			// NOTE: If escape char is not finished, we have to pause and wait for it
			if (i + 2 >= client->in_len)
				break;

			if (client->in_buffer[i+1] != '[') {
				// NOTE: Invalid escape sequence
				i += 1;
				continue;
			}
			char c = client->in_buffer[i + 2];
			handle_client_escaped_char(client, c);
			i += 3;
			continue;
		}

		handle_client_char(client, client->in_buffer[i]);
		i++;
	}
	if (i == 0)
		return;
	if (i < client->in_len) {
		memmove(
			client->in_buffer,
			client->in_buffer + i,
			client->in_len - i
		);
		client->in_len -= i;
	} else {
		// NOTE: We read the whole input buffer
		client->in_len = 0;
	}
}


static void handle_client(fd_set *read_fds, fd_set *write_fds, Client *client) {
	if (!client->connected)
		return;
	
	if (FD_ISSET(client->socket, read_fds)) {
		int n = recv(
			client->socket,
			client->in_buffer + client->in_len,
			sizeof(client->in_buffer) - client->in_len,
			0
		);
		if (n < 0) {
			if (n == EWOULDBLOCK)
				return;
			close(client->socket);
			client->socket = 0;
			client->connected = false;
			return;
		}
		if (n > 0) {
			client->in_len += n;
			handle_client_message(client);
		}
	}
}



void network_update() {
	fd_set read_fds, write_fds;

	int max_fd = init_fd_sets(&read_fds, &write_fds);
	struct timeval tv = {0, 5000};
	if (select(max_fd + 1, &read_fds, &write_fds, NULL, &tv) < 0)
		return;

	accept_clients(&read_fds);

	for (int i = 0; i < PLAYER_COUNT; i++)
		handle_client(&read_fds, &write_fds, &server.clients[i]);
}


Client* client_get(int i) {
	return &server.clients[i];
}


void network_buffer_clear() {
	server.buffer_length = 0;
}

int network_buffer_length() {
	return server.buffer_length;
}

void network_buffer_write(const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	int space = sizeof(server.buffer) - server.buffer_length;
	if (space <= 0) {
		va_end(args);
		return;
	}

	int n = vsnprintf(
		server.buffer + server.buffer_length,
		space,
		fmt, args
	);
	if (n > 0) {
		server.buffer_length += (n < space) ? n : space - 1;
	}
	va_end(args);
	return;
}


int network_send(Client *client) {
	if (server.buffer_length == 0)
		return 0;
	return send(client->socket, server.buffer, server.buffer_length, 0);
}

void network_render_step() {
	for (int i = 0; i < PLAYER_COUNT; i++) {
		Client *client = client_get(i);
		if (!client->connected)
			continue;
		client_render(client);
	}
}
