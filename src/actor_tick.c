#include "actor.h"
#include "game.h"
#include "linalg.h"
#include "player.h"
#include "vision.h"
#include "palette.h"
#include <stdio.h>
#include <stdlib.h>


static inline void try_move(Actor *actor, Vec2i delta) {
	Vec2i new_pos = vec2i_add(actor->position, delta);
	if (game_is_solid(new_pos.x, new_pos.y))
		return;
	actor->position = new_pos;
	gcd_set(&actor->gcd, 500);
}


static void enemy_tick(Actor *actor, double dt) {
	gcd_tick(&actor->gcd, dt);
	if (!gcd_is_free(&actor->gcd))
		return;

	int dir = rand() % 6;
	switch (dir) {
	case 0: try_move(actor, VEC2I_RIGHT); break;
	case 1: try_move(actor, VEC2I_LEFT); break;
	case 2: try_move(actor, VEC2I_UP); break;
	case 3: try_move(actor, VEC2I_DOWN); break;
	default: gcd_set(&actor->gcd, 300); return;
	}
	viewshed_dirty(&actor->viewshed);

	Actor *player = actor_get(game.players[0].actor);
	if (viewshed_contains(&actor->viewshed, player->position)) {
		actor->glyph.fg = COLOR_RED;
	} else {
		actor->glyph.fg = COLOR_YELLOW;
	}
}


void actor_tick(Actor *actor, double dt) {
	switch (actor->kind) {
	case ACTOR_PLAYER: break;
	case ACTOR_ENEMY: enemy_tick(actor, dt); break;
	case ACTOR_ITEM: break;
	}

	viewshed_process(&actor->viewshed, actor->position);
}

