#include "display.h"
#include "config.h"
#include "glyph.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>


#define ANSI_RESET "\x1b[0m"
#define ANSI_CLEAR "\x1b[2J"
#define ANSI_POS(x, y) "\x1b[" #y ";" #y "H"


void display_clear(Display *display) {
	memset(display->z_index, 0, sizeof(display->z_index));
	memset(display->glyphs, 0, sizeof(display->glyphs));
}


void display_set(Display *display, int x, int y, int z, Glyph glyph) {
	if (!display_contains(x, y))
		return;
	if (display->z_index[y][x] > z)
		return;
	display->glyphs[y][x] = glyph;
	display->z_index[y][x] = z;
}


void display_string(Display *display, int x, int y, int z, const char *str, uint8_t fg, uint8_t bg) {
	for (int i = 0; str[i] != '\0'; i++) {
		display_set(display, x + i, y, z, glyph(str[i], fg, bg));
	}
}

