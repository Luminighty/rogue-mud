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


static inline void set_position(Actor *actor, Vec2i new_pos) {
	if (game_is_solid(new_pos.x, new_pos.y))
		return;
	actor->position = new_pos;
	gcd_set(&actor->gcd, 500);
	viewshed_dirty(&actor->viewshed);
}	

static inline void try_move(Actor *actor, Vec2i delta) {
	Vec2i new_pos = vec2i_add(actor->position, delta);
	set_position(actor, new_pos);
}


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
	if (actor->path.count > 0) {
		Vec2i new_pos = da_pop(actor->path);
		set_position(actor, new_pos);
	} else {
		int dir = rand() % 6;
		switch (dir) {
		case 0: try_move(actor, VEC2I_RIGHT); break;
		case 1: try_move(actor, VEC2I_LEFT); break;
		case 2: try_move(actor, VEC2I_UP); break;
		case 3: try_move(actor, VEC2I_DOWN); break;
		default: gcd_set(&actor->gcd, 300); return;
		}
	}
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

