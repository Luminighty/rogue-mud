#ifndef VISION_H
#define VISION_H

#include "map.h"

typedef enum {
	VISION_REVEALED =	1 << 0,
	VISION_VISIBLE =	1 << 1,
} VisionFlag;

typedef struct vision {
	Map *map;
	VisionFlag flags[MAP_SIZE];
} Vision;


static inline bool vision_is_visible(Vision *vision, int x, int y) {
	if (map_contains(vision->map, x, y))
		return vision->flags[map_idx(vision->map, x, y)] & VISION_VISIBLE;
	return false;
}

static inline bool vision_is_revealed(Vision *vision, int x, int y) {
	if (map_contains(vision->map, x, y))
		return vision->flags[map_idx(vision->map, x, y)] & VISION_REVEALED;
	return false;
}

static inline void vision_set_visible(Vision *vision, int x, int y, bool visible) {
	if (!map_contains(vision->map, x, y))
		return;
	if (visible) {
		vision->flags[map_idx(vision->map, x, y)] |= VISION_VISIBLE;
	} else {
		vision->flags[map_idx(vision->map, x, y)] &= ~VISION_VISIBLE;
	}
}


static inline void vision_set_revealed(Vision *vision, int x, int y, bool revealed) {
	if (!map_contains(vision->map, x, y))
		return;
	if (revealed) {
		vision->flags[map_idx(vision->map, x, y)] |= VISION_REVEALED;
	} else {
		vision->flags[map_idx(vision->map, x, y)] &= ~VISION_REVEALED;
	}
}


static inline void vision_clear_visible(Vision *vision) {
	for (int y = 0; y < vision->map->height; y++) {
	for (int x = 0; x < vision->map->width; x++) {
		vision_set_visible(vision, x, y, false);
	}}
}

static inline void vision_reveal_all(Vision *vision) {
	for (int y = 0; y < vision->map->height; y++) {
	for (int x = 0; x < vision->map->width; x++) {
		vision_set_revealed(vision, x, y, true);
	}}
}

#endif // VISION_H
