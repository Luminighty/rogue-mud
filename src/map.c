#include "map.h"
#include <stdio.h>
#include "config.h"
#include "display.h"
#include "glyph.h"
#include "linalg.h"
#include "palette.h"
#include "random.h"
#include "tile.h"
#include "vision.h"


// NOTE: This is to include room walls
static void map_fill_rect(Map *map, Rect2i rect, Tile tile) {
	for (int dx = 0; dx < rect.w; dx++) {
	for (int dy = 0; dy < rect.h; dy++) {
		map_set(map, rect.x + dx, rect.y + dy, tile);
	}}
}


static void map_create_room(Map *map, Room *room) {
	Rect2i rect = room->rect;
	rect.x += 1;
	rect.y += 1;
	rect.w -= 2;
	rect.h -= 2;
	map_fill_rect(map, rect, TILE_FLOOR);
}


static void add_horizontal_corridor(Map *map, int from_x, int to_x, int y) {
	int min = from_x < to_x ? from_x : to_x;
	int max = from_x > to_x ? from_x : to_x;
	for (int x = min; x <= max; x++)
		map_set(map, x, y, TILE_FLOOR);
}


static void add_vertical_corridor(Map *map, int from_y, int to_y, int x) {
	int min = from_y < to_y ? from_y : to_y;
	int max = from_y > to_y ? from_y : to_y;
	for (int y = min; y <= max; y++)
		map_set(map, x, y, TILE_FLOOR);
}


void try_create_room(Map *map) {
	static const int MIN_SIZE = 6;
	static const int MAX_SIZE = 10;
	int w = random_range(MIN_SIZE, MAX_SIZE);
	int h = random_range(MIN_SIZE, MAX_SIZE);
	int x = random_range(1, MAP_WIDTH - w - 1) - 1;
	int y = random_range(1, MAP_HEIGHT - h - 1) - 1;

	Rect2i rect = {.x = x, .y = y, .w = w, .h = h};
	foreach_room(map, i) {
		bool overlaps = rect2i_overlaps(rect, map->rooms[i].rect);
		if (overlaps)
			return;
	}

	printf("New room: " RECT2I_FMT "\n", RECT2I_ARG(rect));
	Room *new_room = &map->rooms[map->room_c++];
	new_room->rect = rect;

	// CORRIDORS
	if (map->room_c >= 2) {
		Vec2i new_center = rect2i_center(rect);
		Vec2i old_center = rect2i_center(map->rooms[map->room_c - 2].rect);
		if (random_range(0, 2) == 1) {
			add_horizontal_corridor(map, old_center.x, new_center.x, old_center.y);
			add_vertical_corridor(map, old_center.y, new_center.y, new_center.x);
		} else {
			add_vertical_corridor(map, old_center.y, new_center.y, old_center.x);
			add_horizontal_corridor(map, old_center.x, new_center.x, new_center.y);
		}
	}

	map_create_room(map, new_room);
}


void map_generate(Map *map) {
	map->width = MAP_WIDTH;
	map->height = MAP_HEIGHT;
	foreach_tile(map, x, y)
		map_set(map, x, y, TILE_WALL);

	for (int i = 0; i < ROOM_MAX; i++)
		try_create_room(map);
}


void map_render(Display *display, Map *map, Vision *vision) {
	foreach_tile(map, x, y) {
		Tile t = map_get(map, x, y);
		Glyph g = tile_glyphs[t];
		if (!vision_is_revealed(vision, x, y))
			continue;
		if (!vision_is_visible(vision, x, y))
			g.fg = COLOR_DARK_GRAY;
		display_set(display, x, y, 1, g);
	}
}


bool map_is_solid(Map *map, int x, int y) {
	if (!map_contains(map, x, y))
		return true;

	return tile_is_solid(map_get(map, x, y));
}

