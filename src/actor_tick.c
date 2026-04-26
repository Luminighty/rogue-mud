#include "actor.h"
#include "darray.h"
#include "game.h"
#include "linalg.h"
#include "map.h"
#include "pathfind.h"
#include "player.h"
#include "vision.h"
#include "palette.h"
#include <stdio.h>
#include <stdlib.h>



static PathCost path_cost(Vec2i pos, void *data) {
	(void)data;
	if (tile_is_solid(map_get(&game->map, pos.x, pos.y)))
		return PATHFIND_BLOCK;
	return 1;
}


static void enemy_chase(Actor *actor, Actor *target) {
	if (actor->path.count > 0 && vec2i_eq(actor->path.items[0], target->position))
		return;
	pathfind_2d(
		&actor->path, actor->position, target->position,
		path_cost, NULL
	);
}


static void enemy_move(Actor *actor) {
	static const double MOVE_GCD = 500;
	Vec2i delta = {0};
	if (actor->path.count > 0) {
		Vec2i new_pos = da_pop(actor->path);
		delta = vec2i_sub(new_pos, actor->position);
	} else {
		int dir = rand() % 6;
		switch (dir) {
		case 0: delta = VEC2I_RIGHT; break;
		case 1: delta = VEC2I_LEFT; break;
		case 2: delta = VEC2I_UP; break;
		case 3: delta = VEC2I_DOWN; break;
		default: return;
		}
	}
	actor_try_move(actor, delta, MOVE_GCD);
}


static void enemy_tick(Actor *actor, double dt) {
	gcd_tick(&actor->gcd, dt);
	if (!gcd_is_free(&actor->gcd))
		return;
	Actor *player = actor_get(game->players[0].actor);
	if (viewshed_contains(&actor->viewshed, player->position)) {
		enemy_chase(actor, player);
	}
	enemy_move(actor);
}


void actor_tick(Actor *actor, double dt) {
	switch (actor->kind) {
	case ACTOR_PLAYER: break;
	case ACTOR_ENEMY: enemy_tick(actor, dt); break;
	case ACTOR_ITEM: break;
	}

	viewshed_process(&actor->viewshed, actor->position);
}

