#ifndef CLIENT_H
#define CLIENT_H

#include "display.h"
#include <stdbool.h>
#include <stdint.h>


typedef struct {
	uint16_t scanline;
	uint8_t fg;
	uint8_t bg;
} ClientTerminal;


typedef struct {
	bool connected;
	int socket;

	char in_buffer[64];
	uint16_t in_len;

	ClientTerminal terminal;
	Display display;
	
	uint16_t player_index;
} Client;


void client_render(Client *client);
void client_set_dirty(Client *client);


#endif // CLIENT_H
