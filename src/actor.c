#include "actor.h"
#include "actors.h"
#include "game.h"
#include "gcd.h"
#include "linalg.h"
#include "map.h"
#include <assert.h>
#include <stdbool.h>


ActorId actor_create_opt(ActorKind kind, ActorParams params) {
	ActorId id = actors_push();
	Actor *actor = actor_get(id);
	params.kind = kind;

	if (params.max_hp == 0)
		params.max_hp = params.hp;
	if (params.hp == 0)
		params.hp = params.max_hp;

	*actor = params;

	int map_index = map_idx(&game->map, params.position.x, params.position.y);
	game->map.blocked[map_index] = params.blocks;

	return id;
}


void actor_destroy(ActorId id) {
	// NOTE: Add additional cleanup here
	actors_remove(id);
}


bool actor_try_move(Actor *actor, Vec2i delta, double gcd) {
	if (vec2i_eq(delta, VEC2I_ZERO))
		return false;

	Vec2i new_pos = vec2i_add(actor->position, delta);
	if (game_is_solid(new_pos.x, new_pos.y))
		return false;

	if (actor->blocks) {
		game->map.blocked[map_idx(&game->map, actor->position.x, actor->position.y)] = false;
		game->map.blocked[map_idx(&game->map, new_pos.x, new_pos.y)] = true;
	}

	actor->position = new_pos;
	gcd_set(&actor->gcd, gcd);
	viewshed_dirty(&actor->viewshed);
	return true;
}
