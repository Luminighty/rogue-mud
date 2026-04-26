#ifndef MAP_H
#define MAP_H

#include "config.h"
#include "display.h"
#include "linalg.h"
#include "tile.h"
#include <stdint.h>
#include <stdbool.h>


#define ROOM_MAX 32

typedef struct {
	Rect2i rect;
} Room;

typedef struct {
	Tile tiles[MAP_SIZE];
	bool blocked[MAP_SIZE];
	uint16_t width;
	uint16_t height;
	Room rooms[ROOM_MAX];
	int room_c;
} Map;


void map_generate(Map *map);
bool map_is_solid(Map *map, int x, int y);

struct vision;
void map_render(Display *display, Map *map, struct vision *vision);

#define foreach_room(map, i) for (int i = 0; i < (map)->room_c; i++)
#define foreach_tile(map, x, y) \
	for (int y = 0; y < MAP_HEIGHT; y++) \
	for (int x = 0; x < MAP_WIDTH; x++)

static inline int map_idx(Map *map, int x, int y) {
	return x + map->width * y;
}

static inline bool map_contains(Map *map, int x, int y) {
	return x >= 0 && y >= 0 && x < map->width && y < map->height;
}
static inline void map_set(Map *map, int x, int y, Tile tile) {
	if (map_contains(map, x, y))
		map->tiles[map_idx(map, x, y)] = tile;
}
static inline Tile map_get(Map *map, int x, int y) {
	if (map_contains(map, x, y))
		return map->tiles[map_idx(map, x, y)];
	return TILE_NONE;
}


#endif // MAP_H
