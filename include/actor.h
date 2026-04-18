#ifndef ACTOR_H
#define ACTOR_H


#include "actor_id.h"
#include "gcd.h"
#include "glyph.h"
#include "linalg.h"
#include "vision.h"
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
	char *name;

	Viewshed viewshed;
} Actor;



typedef Actor ActorParams;
#define actor_create(kind, ...) actor_create_opt(kind, ((ActorParams){ __VA_ARGS__ }))
ActorId actor_create_opt(ActorKind kind, ActorParams params);

void actor_destroy(ActorId id);

void actor_tick(Actor *actor, double dt);


#endif // ACTOR_H
