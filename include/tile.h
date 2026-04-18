#ifndef TILE_H
#define TILE_H

#include "glyph.h"

typedef enum {
	TILE_NONE,
	TILE_FLOOR,
	TILE_WALL,
	TILE_SIZE,
} Tile;


typedef enum {
	TILE_FLAG_SOLID  = 1 << 0,
	TILE_FLAG_OPAQUE = 1 << 1,
} TileFlag;



extern const TileFlag tile_flags[TILE_SIZE];
extern const Glyph tile_glyphs[TILE_SIZE];

#define tile_is_solid(tile) tile_flags[(tile)] & TILE_FLAG_SOLID
#define tile_is_opaque(tile) tile_flags[(tile)] & TILE_FLAG_OPAQUE



#endif // TILE_H
