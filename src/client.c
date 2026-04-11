#include "client.h"
#include "ansi.h"
#include "config.h"
#include "display.h"
#include "glyph.h"
#include "network.h"
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>
#include "tcp_server.h"


static const char accept_message[] = 
	ANSI_CURSOR_HIDE ANSI_CLEAR ANSI_BOLD;

static const char close_message[] = 
	ANSI_CURSOR_SHOW ANSI_RESET "\n\n\tGoodbye!\n\n";


static void client_render_row(ClientTerminal *term, Display *display, int row) {
	for (int x = 0; x < DISPLAY_WIDTH; x++) {
		Glyph g = display_get(display, x, row);
		if (x == 0 || term->fg != g.fg) {
			network_buffer_write(Glyph_FMT_FG, g.fg);
			term->fg = g.fg;
		}
		if (x == 0 || term->bg != g.bg) {
			network_buffer_write(Glyph_FMT_BG, g.bg);
			term->bg = g.bg;
		}
		network_buffer_write_glyph(g.c);
	}
}


void client_render(Client *client) {
	ClientTerminal *term = &client->terminal;

	if (term->resized) {
		term->resized = false;
		network_buffer_write(ANSI_RESET ANSI_CLEAR ANSI_BOLD);
		term->fg = 0;
		term->bg = 0;
		term->scanline = 0;
	}

	while (term->scanline < DISPLAY_HEIGHT) {
		network_buffer_write(ANSI_POS("%d", "0"), term->scanline + 1);
		client_render_row(term, &client->display, term->scanline);

		int sent = network_send(client);
		int buffer_len = network_buffer_length();
		network_buffer_clear();
		if (sent < buffer_len) {
			return;
		}
		term->scanline++;
	}
}


void client_set_dirty(Client *client) {
	client->terminal.scanline = 0;
}


int client_send(Client *client, const char *message, size_t length) {
	if (!client->connected) return -1;
	int res = app_tcp_send(client->socket, message, length);
	if (res < 0 && errno == EPIPE) {
		printf("Client %d disconnected.\n", client->player_index);
		client_close(client);
	}
	return res;
}


void client_close(Client *client) {
	if (!client->connected) return;
	printf("Closing client %d.\n", client->socket);
	app_tcp_send(client->socket, close_message, sizeof(close_message));
	app_tcp_close(client->socket);
	client->connected = false;
	client->socket = 0;
}

void client_send_accept_message(Client *client) {
	client_send(client, accept_message, sizeof(accept_message));
}

