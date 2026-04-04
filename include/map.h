#ifndef MAP_H
#define MAP_H

#include "config.h"
#include "display.h"
#include <stdint.h>
#include <stdbool.h>


typedef enum {
	TILE_NONE,
	TILE_FLOOR,
	TILE_WALL,
	TILE_SIZE,
} Tile;

typedef struct {
	Tile tiles[MAP_SIZE];
	uint16_t width;
	uint16_t height;
} Map;


void map_generate(Map *map);
bool map_is_solid(Map *map, int x, int y);

struct vision;
void map_render(Display *display, Map *map, struct vision *vision);

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
