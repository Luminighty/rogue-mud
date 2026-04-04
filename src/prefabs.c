#include "prefabs.h"
#include "actor.h"
#include "palette.h"


ActorId goblin_create(Vec2i position) {
	return actor_create(
		ACTOR_ENEMY,
		.position = position,
		.glyph = glyph('g', COLOR_RED, COLOR_BLACK),
		.z = 8
	);
}


ActorId orc_create(Vec2i position) {
	return actor_create(
		ACTOR_ENEMY,
		.position = position,
		.glyph = glyph('o', COLOR_RED, COLOR_BLACK),
		.z = 8
	);
}


ActorId potion_create(Vec2i position) {
	return actor_create(
		ACTOR_ITEM,
		.position = position,
		.glyph = glyph(0xAD, COLOR_PINK, COLOR_BLACK),
		.z = 5
	);
}


ActorId scroll_create(Vec2i position) {
	return actor_create(
		ACTOR_ITEM,
		.position = position,
		.glyph = glyph('(', COLOR_CYAN, COLOR_BLACK),
		.z = 5
	);
}

