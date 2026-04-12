#include "tile.h"

const TileFlag tile_flags[TILE_SIZE] = {
	[TILE_NONE] = TILE_FLAG_SOLID | TILE_FLAG_OPAQUE,
	[TILE_WALL] = TILE_FLAG_SOLID | TILE_FLAG_OPAQUE,
};


const Glyph tile_glyphs[TILE_SIZE] = {
	[TILE_NONE] = glyph(' ', COLOR_BLACK, COLOR_BLACK),
	[TILE_WALL] = glyph('#', COLOR_GREEN, COLOR_BLACK),
	[TILE_FLOOR] = glyph('.', COLOR_TEAL, COLOR_BLACK),
};

