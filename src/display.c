#include "display.h"
#include "glyph.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "palette.h"


void display_clear(Display *display) {
	memset(display->z_index, 0, sizeof(display->z_index));

	Glyph base = glyph(0, COLOR_BLACK, COLOR_BLACK);
	for (int y = 0; y < DISPLAY_HEIGHT; y++)
	for (int x = 0; x < DISPLAY_WIDTH; x++)
		display->glyphs[y][x] = base;
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

