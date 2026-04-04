#ifndef CLIENT_H
#define CLIENT_H

#include "display.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


typedef struct {
	uint16_t scanline;
	uint8_t fg;
	uint8_t bg;

	uint16_t width;
	uint16_t height;
	bool resized;
} ClientTerminal;


typedef struct {
	bool connected;
	bool should_exit;
	int socket;

	uint8_t in_buffer[64];
	uint16_t in_len;

	ClientTerminal terminal;
	Display display;
	
	uint16_t player_index;
} Client;


void client_render(Client *client);
void client_set_dirty(Client *client);
void client_send_accept_message(Client *client);
void client_close(Client *client);
int client_send(Client *client, const char *message, size_t length);


#endif // CLIENT_H
