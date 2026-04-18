#include "vision.h"
#include "darray.h"
#include "fov.h"
#include "linalg.h"
#include "game.h"


static bool is_opaque(int x, int y, void *_data) {
	(void)(_data); // unused
	Tile tile = map_get(&game.map, x, y);
	return tile_is_opaque(tile);
}


static void on_visible(int x, int y, void *data) {
	Viewshed *viewshed = data;
	// TODO: Check for duplicates duh.. (though maybe I just don't care)
	da_push(viewshed->visible_tiles, vec2i(x, y));
}



void viewshed_process(Viewshed *viewshed, Vec2i position) {
	if (!viewshed->dirty)
		return;
	viewshed->dirty = false;
	da_clear(viewshed->visible_tiles);
	fov_2d(
		position.x, position.y, 5,
		is_opaque, on_visible,
		viewshed
	);
}

bool viewshed_contains(Viewshed *viewshed, Vec2i position) {
	foreach_da(viewshed->visible_tiles, i) {
		Vec2i pos = viewshed->visible_tiles.items[i];
		if (position.x == pos.x && position.y == pos.y)
			return true;
	}
	return false;
}

