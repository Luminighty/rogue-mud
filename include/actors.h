#ifndef ACTORS_H
#define ACTORS_H


#include "actor.h"
#include "actor_id.h"
#include "config.h"
#include "display.h"
#include <stdbool.h>
#include <stdint.h>


typedef struct {
	Actor actors[ACTOR_COUNT];

	uint32_t generation[ACTOR_COUNT];
	uint32_t next_free[ACTOR_COUNT];
	bool alive[ACTOR_COUNT];

	uint32_t first_free;
	uint32_t max_index;
} Actors;


void actors_init();
ActorId actors_push();
void actors_remove(ActorId id);
Actor *actor_get(ActorId id);


struct vision;
void actors_tick(Actors *actors, double dt);
void actors_render(Display *display, Actors *actors, struct vision *context);


ActorId actors_begin(Actors *actors);
bool actors_has_next(Actors *actors, ActorId *actor);
void actors_next(Actors *actors, ActorId *actor);

#define foreach_actor(actor, actors) \
	for (\
		ActorId actor = actors_begin(actors);\
		actors_has_next(actors, &actor);\
		actors_next(actors, &actor)\
	)


#endif // ACTORS_H
