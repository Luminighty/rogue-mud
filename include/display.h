#ifndef DISPLAY_H
#define DISPLAY_H


#include "config.h"
#include "glyph.h"

#include <stdbool.h>
#include <stdint.h>

#define Z_SKIP 0

typedef struct {
	Glyph glyphs[DISPLAY_HEIGHT][DISPLAY_WIDTH];
	uint8_t z_index[DISPLAY_HEIGHT][DISPLAY_WIDTH];

	// TODO: Start using these once players have their own camera, maybe even pull vec2i
	int offset_x;
	int offset_y;
} Display;


void display_clear(Display *display);
void display_set(Display *display, int x, int y, int z, Glyph glyph);
int display_string(Display *display, int x, int y, int z, const char *str, uint8_t fg, uint8_t bg);
int display_int(Display *display, int x, int y, int z, int value, uint8_t fg, uint8_t bg);
void display_box(Display *display, int x, int y, int width, int height);

static inline bool display_contains(int x, int y) {
	return x >= 0 && y >= 0 && x < DISPLAY_WIDTH && y < DISPLAY_HEIGHT;
}


static inline Glyph display_get(Display *display, int x, int y) {
	if (!display_contains(x, y))
		return (Glyph){0};
	return display->glyphs[y][x];
}


#endif // DISPLAY_H
