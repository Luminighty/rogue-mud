#include "map.h"
#include "config.h"
#include "display.h"
#include "glyph.h"
#include "palette.h"
#include "vision.h"


static void add_room(Map *map, int x, int y, int w, int h) {
	for (int dx = 0; dx < w; dx++) {
	for (int dy = 0; dy < h; dy++) {
		map_set(map, x + dx, y + dy, TILE_FLOOR);
	}}
}


static void add_corridor_x(Map *map, int from_x, int from_y, int to_x, int to_y) {
	for (int x = from_x; x <= to_x; x++)
		map_set(map, x, from_y, TILE_FLOOR);
	for (int y = from_y; y <= to_y; y++)
		map_set(map, to_x, y, TILE_FLOOR);
}


static void add_corridor_y(Map *map, int from_x, int from_y, int to_x, int to_y) {
	for (int y = from_y; y <= to_y; y++)
		map_set(map, from_x, y, TILE_FLOOR);
	for (int x = from_x; x <= to_x; x++)
		map_set(map, x, to_y, TILE_FLOOR);
}


static const Glyph tile_glyphs[TILE_SIZE] = {
	[TILE_NONE] = glyph(' ', COLOR_BLACK, COLOR_BLACK),
	[TILE_WALL] = glyph('#', COLOR_GREEN, COLOR_BLACK),
	[TILE_FLOOR] = glyph('.', COLOR_GRAY, COLOR_BLACK),
};


void map_generate(Map *map) {
	map->width = MAP_WIDTH;
	map->height = MAP_HEIGHT;
	for (int y = 0; y < map->height; y++) {
	for (int x = 0; x < map->width; x++) {
		map_set(map, x, y, TILE_WALL);
	}}
	add_room(map, 3,   1, 10, 10);
	add_room(map, 15, 13, 5, 5);
	add_room(map, 4,  15, 7, 5);
	add_room(map, 22, 17, 15, 4);
	add_room(map, 24,  2, 10, 13);
	add_room(map, 40,  6, 5, 5);
	add_corridor_x(map, 6,   4, 17, 15);
	add_corridor_x(map, 7,  16, 17, 16);
	add_corridor_y(map, 17, 15, 30, 19);
	add_corridor_x(map, 25,  8, 29, 18);
	add_corridor_x(map, 29,  3, 43, 9);
}


void map_render(Display *display, Map *map, Vision *vision) {
	for (int y = 0; y < MAP_HEIGHT; y++) {
	for (int x = 0; x < MAP_WIDTH; x++) {
		Tile t = map_get(map, x, y);
		Glyph g = tile_glyphs[t];
		if (!vision_is_revealed(vision, x, y))
			continue;
		if (!vision_is_visible(vision, x, y))
			g.fg = COLOR_DARK_GRAY;
		display_set(display, x, y, 1, g);
	}}
}


bool map_is_solid(Map *map, int x, int y) {
	if (!map_contains(map, x, y))
		return true;
	switch (map_get(map, x, y)) {
	case TILE_WALL: return true;
	default: return false;
	}
}

