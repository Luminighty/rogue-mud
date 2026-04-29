#include "network.h"
#include "client.h"
#include "client_telnet.h"
#include "config.h"
#include "cp437.h"
#include "game.h"
#include "key.h"

#include "tcp_server.h"

#include <assert.h>
#include <errno.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>


static Server server = {0};

static const char telnet_mode[] = {
	255, 251, 1,	// IAC WILL ECHO
	255, 251, 3,	// IAC WILL (SUPPR GO AHEAD)
	255, 254, 34,	// IAC DONT LINEMODE
	255, 253, 31,	// IAC DO NAWS
};


void network_init() {
	server.socket = app_tcp_server_listen(NETWORK_PORT, 128);
}


void network_destroy() {
	printf("NETWORK destroy.\n");
	for (int i = 0; i < PLAYER_COUNT; i++)
		client_close(&server.clients[i]);
	app_tcp_close(server.socket);
}


static inline Client *push_client() {
	for (int i = 0; i < PLAYER_COUNT; i++) {
		if (!server.clients[i].connected) {
			memset(&server.clients[i], 0, sizeof(Client));
			server.clients[i].connected = true;
			server.clients[i].player_index = i;
			return &server.clients[i];
		}
	}
	assert(0);
}


static inline void accept_clients() {
	while (1) {
		int socket = app_tcp_accept(server.socket);
		if (socket < 0) {
			if (errno != EWOULDBLOCK && errno != EAGAIN)
				perror("Accept error");
			return;
		}
		Client *client = push_client();
		client->socket = socket;
		app_tcp_set_non_blocking(socket);

		client_send(client, telnet_mode, sizeof(telnet_mode));
		client_send_accept_message(client);
	}
}


static int handle_client_escaped_char(Client *client, char c) {
	switch (c) {
	case 'A': return KEY_UP;
	case 'B': return KEY_DOWN;
	case 'C': return KEY_RIGHT;
	case 'D': return KEY_LEFT;
	default: // UNKNOWN
		return 0;
	}
}
static void handle_client_char(Client *client, char c, char modifier) {
	KeyStroke key = {.key = c};
	if (modifier != 0) {
		modifier -= '0' - 1;
		key.shift = modifier & 0b001;
		key.alt   = modifier & 0b010;
		key.ctrl  = modifier & 0b100;
	}
	
	game_input(client->player_index, key);
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
	client_telnet_receive(client);
}


static void client_recv(Client *client) {
	if (!client->connected)
		return;
	
	int n = app_tcp_recv(
		client->socket,
		client->in_buffer + client->in_len,
		sizeof(client->in_buffer) - client->in_len
	);
	if (n < 0) {
		if (errno == EWOULDBLOCK || errno == EAGAIN)
			return;
		perror("Client recv error.");
		close(client->socket);
		client->socket = 0;
		client->connected = false;
		return;
	}
	if (n > 0) {
		// for (int i = 0; i < n; i++)
		// 	printf("Received '%d'\n", client->in_buffer[client->in_len + i]);
		client->in_len += n;
		handle_client_message(client);
	}
}


void network_update() {
	accept_clients();
	for (int i = 0; i < PLAYER_COUNT; i++)
		client_recv(&server.clients[i]);
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
		server.buffer_length +=
			(n < space_remaining)
			? n 
			: space_remaining - 1;
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

