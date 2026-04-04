#ifndef NETWORK_H
#define NETWORK_H


#include "client.h"
#include "config.h"
#include <stdint.h>

#define SERVER_BUFFER_SIZE 8192

typedef struct {
	int socket;
	Client clients[PLAYER_COUNT];

	char buffer[SERVER_BUFFER_SIZE];
	uint16_t buffer_length;
} Server;


void network_init();
void network_destroy();
void network_update();
Client* client_get(int i);


void network_render_step();
void network_buffer_clear();
void network_buffer_write(const char *fmt, ...);
void network_buffer_write_glyph(uint8_t c);
int network_buffer_length();
int network_send(Client *client);


#endif // NETWORK_H
