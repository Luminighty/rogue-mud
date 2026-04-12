#include "display.h"
#include "glyph.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
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
	if (display->z_index[y][x] > z && z != Z_SKIP)
		return;
	display->glyphs[y][x] = glyph;
	display->z_index[y][x] = z;
}


int display_string(Display *display, int x, int y, int z, const char *str, uint8_t fg, uint8_t bg) {
	int i = 0;
	for (; str[i] != '\0'; i++) {
		display_set(display, x + i, y, z, glyph(str[i], fg, bg));
	}
	return i;
}

int display_int(Display *display, int x, int y, int z, int value, uint8_t fg, uint8_t bg) {
	char buffer[16];
	int len = sprintf(buffer, "%d", value);
	return display_string(display, x, y, z, buffer, fg, bg);
}

void display_box(Display *display, int x, int y, int width, int height) {
	uint8_t fg = COLOR_WHITE;
	uint8_t bg = COLOR_BLACK;
	Glyph g_h = glyph(0xC4, fg, bg);
	Glyph g_v = glyph(0xB3, fg, bg);
	Glyph g_tl = glyph(0xDA, fg, bg);
	Glyph g_tr = glyph(0xBF, fg, bg);
	Glyph g_bl = glyph(0xC0, fg, bg);
	Glyph g_br = glyph(0xD9, fg, bg);
	int xmax = x + width - 1;
	int ymax = y + height - 1;
	for (int i = 1; i < width - 1; i++) {
		display_set(display, x + i, y   , Z_SKIP, g_h);
		display_set(display, x + i, ymax, Z_SKIP, g_h);
	}
	for (int i = 1; i < height - 1; i++) {
		display_set(display, x   , y + i, Z_SKIP, g_v);
		display_set(display, xmax, y + i, Z_SKIP, g_v);
	}
	display_set(display, x   , y   , Z_SKIP, g_tl);
	display_set(display, xmax, y   , Z_SKIP, g_tr);
	display_set(display, x   , ymax, Z_SKIP, g_bl);
	display_set(display, xmax, ymax, Z_SKIP, g_br);
}
