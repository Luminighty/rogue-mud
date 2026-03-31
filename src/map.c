#include "map.h"
#include "config.h"
#include "display.h"
#include "glyph.h"
#include "palette.h"

#include <string.h>


static void add_room(Map *map, int x, int y, int w, int h) {
	for (int dx = 0; dx < w; dx++) {
	for (int dy = 0; dy < h; dy++) {
		map_set(map, x + dx, y + dy, TILE_FLOOR);
	}}
}

static void add_corridor(Map *map, int from_x, int from_y, int to_x, int to_y) {
	for (int x = from_x; x <= to_x; x++)
		map_set(map, x, from_y, TILE_FLOOR);
	for (int y = from_y; y <= to_y; y++)
		map_set(map, to_x, y, TILE_FLOOR);
}


static const Glyph tile_glyphs[TILE_SIZE] = {
	[TILE_NONE] = glyph(' ', COLOR_BLACK, COLOR_BLACK),
	[TILE_WALL] = glyph('#', COLOR_YELLOW, COLOR_BLACK),
	[TILE_FLOOR] = glyph('.', COLOR_GRAY, COLOR_BLACK),
};


void map_generate(Map *map) {
	map->width = MAP_WIDTH;
	map->height = MAP_HEIGHT;
	for (int y = 0; y < map->height; y++) {
	for (int x = 0; x < map->width; x++) {
		map_set(map, x, y, TILE_WALL);
	}}
	add_room(map, 3, 3, 10, 10);
	add_room(map, 15, 15, 5, 5);
	add_corridor(map, 6, 6, 17, 17);
}


void map_render(Display *display, Map *map) {
	for (int y = 0; y < MAP_HEIGHT; y++) {
	for (int x = 0; x < MAP_WIDTH; x++) {
		Tile t = map_get(map, x, y);
		display_set(display, x, y, 1, tile_glyphs[t]);
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

