#include "network.h"
#include "client.h"
#include "config.h"
#include "cp437.h"
#include "game.h"
#include "key.h"

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>


static Server server = {0};

static const char telnet_mode[] = {
	255, 251, 1,	// IAC WILL ECHO
	255, 251, 3,	// IAC WILL (SUPPR GO AHEAD)
	255, 254, 34,	// IAC DONT LINEMODE
	255, 253, 31,	// IAC DO NAWS
};


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
	for (int i = 0; i < PLAYER_COUNT; i++)
		client_close(&server.clients[i]);
	close(server.socket);
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

		client_send(client, telnet_mode, sizeof(telnet_mode));
		client_send_accept_message(client);
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

// NOTE: Will have to create some lexer logic for this probably once I want to use more telnet features
static const uint8_t TELNET_IAC = 255;
static int handle_telnet_command(Client *client, int i) {
	static const uint8_t TELNET_SB = 250;
	static const uint8_t TELNET_NAWS = 31;
	static const uint8_t TELNET_SE = 240;

	if (i + 2 >= client->in_len)
		return -1;
	if (client->in_buffer[i+1] != TELNET_SB) {
		printf("Not SB %d\n", client->in_buffer[i+1]);
		return 1;
	}
	if (client->in_buffer[i+2] != TELNET_NAWS) {
		printf("Not naws\n");
		return 2;
	}

	if (i + 8 >= client->in_len) {
		printf("Not long enough\n");
		return -1;
	}
	uint16_t width_high = client->in_buffer[i+3];
	uint16_t width_low = client->in_buffer[i+4];
	uint16_t height_high = client->in_buffer[i+5];
	uint16_t height_low = client->in_buffer[i+6];

	if (client->in_buffer[i+7] != TELNET_IAC) {
		printf("Not iac\n");
		return 7;
	}
	if (client->in_buffer[i+8] != TELNET_SE) {
		printf("Not se\n");
		return 8;
	}

	client->terminal.width = (width_high << 8) | width_low;
	client->terminal.height = (height_high << 8) | height_low;
	client->terminal.resized = true;

	printf("Terminal dim: %d;%d\n", client->terminal.width, client->terminal.height);
	return 8;
}

static void handle_client_message(Client *client) {
	static const char ESCAPE_CHAR = 27;
	static const char ESCAPE_CHAR_NEXT = '[';

	int i = 0;
	while(i < client->in_len) {
		uint8_t c = client->in_buffer[i];
		if (c == TELNET_IAC) {
			int n = handle_telnet_command(client, i);
			if (n < 0)
				break;
			i += n;
			continue;
		}
			
		if (c == ESCAPE_CHAR) {
			if (i + 2 >= client->in_len)
				break;
			if (client->in_buffer[i+1] != ESCAPE_CHAR_NEXT) {
				i += 1;
				continue;
			}
			char c = client->in_buffer[i + 2];
			handle_client_escaped_char(client, c);
			i += 3;
			continue;
		}

		handle_client_char(client, c);
		i++;
	}
	if (i == 0)
		return;
	if (i < client->in_len) {
		// NOTE: We partially read the buffer, shift it all to the left
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
			// NOTE: Some error happened
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

#define safe_append_char(c) \
	if (server.buffer_length < sizeof(server.buffer)) \
		server.buffer[server.buffer_length++] = c;

void network_buffer_write_glyph(uint8_t c) {
	const char *utf8 = cp437_to_utf8[c];

	for (int i = 0; utf8[i] != '\0'; i++) {
		if (server.buffer_length >= sizeof(server.buffer))
			break;
		server.buffer[server.buffer_length++] = utf8[i];
	}
}

#undef safe_append_char


void network_buffer_write(const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	int space_remaining = sizeof(server.buffer) - server.buffer_length;
	if (space_remaining <= 0) {
		va_end(args);
		return;
	}

	int n = vsnprintf(
		server.buffer + server.buffer_length,
		space_remaining,
		fmt, args
	);
	if (n > 0) {
		server.buffer_length += (n < space_remaining) ? n : space_remaining - 1;
	}
	va_end(args);
	return;
}


int network_send(Client *client) {
	if (server.buffer_length == 0)
		return 0;
	return client_send(client, server.buffer, server.buffer_length);
}


void network_render_step() {
	for (int i = 0; i < PLAYER_COUNT; i++) {
		Client *client = client_get(i);
		if (!client->connected)
			continue;
		client_render(client);
	}
}

