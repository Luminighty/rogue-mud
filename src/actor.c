#include "actor.h"
#include "actors.h"
#include <assert.h>
#include <stdbool.h>


ActorId actor_create_opt(ActorKind kind, ActorParams params) {
	ActorId id = actors_push();
	Actor *actor = actor_get(id);

	params.kind = kind;
	*actor = params;

	return id;
}


void actor_destroy(ActorId id) {
	// NOTE: Add additional cleanup here
	actors_remove(id);
}
