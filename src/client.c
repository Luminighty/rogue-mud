#include "client.h"
#include "config.h"
#include "display.h"
#include "glyph.h"
#include "network.h"

#define ANSI_CURSOR_HIDE "\033[?25l"
#define ANSI_POS_FMT "\x1b[%d;%dH"
#define ANSI_POS_ARG(x, y) (y), (x)



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
		network_buffer_write("%c", g.c);
	}
}


void client_render(Client *client) {
	ClientTerminal *term = &client->terminal;
	network_buffer_write(ANSI_CURSOR_HIDE);

	while (term->scanline < DISPLAY_HEIGHT) {

		network_buffer_write(ANSI_POS_FMT, ANSI_POS_ARG(0, term->scanline));
		client_render_row(term, &client->display, term->scanline);

		int sent = network_send(client);
		network_buffer_clear();
		if (sent < network_buffer_length()) {
			return;
		}

		term->scanline++;
	}
}


void client_set_dirty(Client *client) {
	client->terminal.scanline = 0;
}

