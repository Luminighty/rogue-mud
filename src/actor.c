#include "actor.h"
#include "config.h"
#include "game.h"
#include "vision.h"
#include "linalg.h"
#include <assert.h>
#include <stdbool.h>
#include <string.h>


void actors_init() {
	game.actors.max_index = 1;
}

ActorId _actor_create(ActorKind kind, ActorParams params) {
	uint16_t idx = game.actors.first_free;
	if (idx == 0) {
		assert(game.actors.max_index < ACTOR_COUNT);
		idx = game.actors.max_index++;
	} else {
		idx = game.actors.first_free;
		game.actors.first_free = game.actors.next_free[idx];
		game.actors.next_free[idx] = 0;
	}
	uint16_t gen = ++game.actors.generation[idx];
	game.actors.alive[idx] = true;

	memset(&game.actors.actors[idx], 0, sizeof(Actor));

	params.kind = kind;
	game.actors.actors[idx] = params;

	return (ActorId){.gen = gen, .index = idx};
}


static inline bool is_alive(ActorId id) {
	return game.actors.alive[id.index] &&
		game.actors.generation[id.index] == id.gen;
}


void actor_destroy(ActorId id) {
	if (!is_alive(id))
		return;
	game.actors.alive[id.index] = false;
	game.actors.next_free[id.index] = game.actors.first_free;
	game.actors.first_free = id.index;
}


Actor *actor_get(ActorId id) {
	if (!is_alive(id)) {
		assert(0);
		return &game.actors.actors[0];
	}
	return &game.actors.actors[id.index];
}


void actors_tick(Actors *actors, double dt) {
	foreach_actor(id, actors)
		actor_tick(actor_get(id), dt);
}


void actors_render(Display *display, Actors *actors, Vision *context) {
	foreach_actor(id, actors) {
		Actor *actor = actor_get(id);
		Vec2i pos = actor->position;
		if (!vision_is_visible(context, pos.x, pos.y))
			continue;
		display_set(
			display,
			pos.x, pos.y,
			actor->z, actor->glyph
		);
	}
}


ActorId actors_begin(Actors *actors) {
	ActorId actor = {0};
	actors_next(actors, &actor);
	return actor;
}


void actors_next(Actors *actors, ActorId *actor) {
	uint16_t index = actor->index;
	while (1) {
		index++;
		if (index >= actors->max_index) {
			actor->index = index;
			actor->gen = 0;
			return;
		}
		if (!actors->alive[index])
			continue;
		actor->index = index;
		actor->gen = actors->generation[index];
		return;
	}
}


bool actors_has_next(Actors *actors, ActorId *actor) {
	return actor->index < actors->max_index;
}

