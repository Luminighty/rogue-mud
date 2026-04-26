#include "prefabs.h"
#include "actor.h"
#include "palette.h"
#include "vision.h"


ActorId goblin_create(Vec2i position) {
	return actor_create(
		ACTOR_ENEMY,
		.position = position,
		.glyph = glyph('g', COLOR_RED, COLOR_BLACK),
		.z = 8,
		.name = "Goblin",
		.viewshed = viewshed_create(6),
		.blocks = true,

		.hp = 8,
		.power = 2,
		.defense = 0,
	);
}


ActorId orc_create(Vec2i position) {
	return actor_create(
		ACTOR_ENEMY,
		.position = position,
		.glyph = glyph('o', COLOR_RED, COLOR_BLACK),
		.z = 8,
		.name = "Orc",
		.viewshed = viewshed_create(8),
		.blocks = true,

		.hp = 16,
		.power = 4,
		.defense = 1,
	);
}


ActorId potion_create(Vec2i position) {
	return actor_create(
		ACTOR_ITEM,
		.position = position,
		.glyph = glyph(0xAD, COLOR_PINK, COLOR_BLACK),
		.z = 5,
		.name = "Health Potion"
	);
}


ActorId scroll_create(Vec2i position) {
	return actor_create(
		ACTOR_ITEM,
		.position = position,
		.glyph = glyph('(', COLOR_CYAN, COLOR_BLACK),
		.z = 5,
		.name = "Magic Scroll"
	);
}

