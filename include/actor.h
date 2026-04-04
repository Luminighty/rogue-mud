#ifndef ACTOR_H
#define ACTOR_H


#include "config.h"
#include "display.h"
#include "gcd.h"
#include "glyph.h"
#include "linalg.h"
#include <stdbool.h>
#include <stdint.h>


typedef enum {
	ACTOR_PLAYER,
	ACTOR_ENEMY,
	ACTOR_ITEM,
} ActorKind;


typedef struct actor {
	ActorKind kind;
	Glyph glyph;
	GCD gcd;
	Vec2i position;
	int z;
	int hp;
} Actor;


typedef union {
	uint64_t id;
	struct {
		uint32_t index;
		uint32_t gen;
	};
} ActorId;


typedef struct {
	uint32_t generation;
	bool active;
} ActorMeta;


typedef struct {
	Actor actors[ACTOR_COUNT];

	uint32_t generation[ACTOR_COUNT];
	uint32_t next_free[ACTOR_COUNT];
	bool alive[ACTOR_COUNT];

	uint32_t first_free;
	uint32_t max_index;
} Actors;


void actors_init();

typedef Actor ActorParams;
#define actor_create(kind, ...) _actor_create(kind, ((ActorParams){ __VA_ARGS__ }))
ActorId _actor_create(ActorKind kind, ActorParams params);
void actor_destroy(ActorId id);
Actor *actor_get(ActorId id);


struct vision;
void actor_tick(Actor *actor, double dt);
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


#endif // ACTOR_H
