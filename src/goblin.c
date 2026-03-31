#include "goblin.h"
#include "game.h"
#include "gcd.h"
#include "palette.h"
#include <stdlib.h>


static inline Goblin *find_goblin_spot() {
	for (int i = 0; i < 12; i++)
		if (!game.goblins[i].active)
			return &game.goblins[i];
	return &game.goblins[0];
}

void goblin_init(int x, int y) {
	Goblin *goblin = find_goblin_spot();
	goblin->active = true;
	goblin->x = x;
	goblin->y = y;
	goblin->hp = 5;
}

static inline void try_move(Goblin *goblin, int dx, int dy) {
	int new_x = goblin->x + dx;
	int new_y = goblin->y + dy;
	if (game_is_solid(new_x, new_y))
		return;
	goblin->x = new_x;
	goblin->y = new_y;
	gcd_set(&goblin->gcd, 500);
}

void goblin_tick(Goblin *goblin, double dt) {
	gcd_tick(&goblin->gcd, dt);
	if (!gcd_is_free(&goblin->gcd))
		return;

	int dir = rand() % 6;
	switch (dir) {
	case 0: try_move(goblin, 1, 0); break;
	case 1: try_move(goblin, -1, 0); break;
	case 2: try_move(goblin, 0, 1); break;
	case 3: try_move(goblin, 0, -1); break;
	default: gcd_set(&goblin->gcd, 300); break;
	}
}

void goblin_render(Display *display, Goblin *goblin) {
	if (!goblin->active)
		return;
	display_set(
		display, 
		goblin->x, goblin->y, 8, 
		glyph(
			'g',
			COLOR_RED,
			COLOR_BLACK
		)
	);
}
