#ifndef PREFAB_H
#define PREFAB_H


#include "actor_id.h"
#include "linalg.h"


ActorId goblin_create(Vec2i position);
ActorId orc_create(Vec2i position);
ActorId potion_create(Vec2i position);
ActorId scroll_create(Vec2i position);


#endif // PREFAB_H
