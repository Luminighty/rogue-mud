#include "player.h"
#include "actor.h"
#include "fov.h"
#include "game.h"
#include "gcd.h"
#include "glyph.h"
#include "linalg.h"
#include "map.h"
#include "palette.h"
#include "tile.h"
#include <assert.h>
#include <string.h>


static const double GCD_MOVE = 0.1 * 1000;


static void update_player_fov(Player *player, Vec2i position);


void player_init(Player *player, Vec2i position, char *name) {
	memset(player, 0, sizeof(Player));

	strncpy(player->name, name, sizeof(player->name));

	player->active = true;
	player->actor = actor_create(
		ACTOR_PLAYER,
		.hp = 30,
		.position = position,
		.glyph = glyph('@', COLOR_YELLOW, COLOR_BLACK),
		.z = 100,
		.name = player->name,
	);
	player->vision.map = &game.map;

	// vision_reveal_all(&player->vision);
	update_player_fov(player, position);
}


void player_set_action(Player *player, PlayerAction action) {
	player->action = action;
}

static inline void try_move(Player *player, Actor *actor, Vec2i delta) {
	Vec2i new_pos = vec2i_add(actor->position, delta);
	if (game_is_solid(new_pos.x, new_pos.y))
		return;
	actor->position = new_pos;
	gcd_set(&actor->gcd, GCD_MOVE);
	update_player_fov(player, new_pos);
}


static inline void player_act(Player *player, Actor *actor) {
	switch (player->action) {
	case ACTION_MOVE_UP:	try_move(player, actor, VEC2I_UP); break;
	case ACTION_MOVE_DOWN:	try_move(player, actor, VEC2I_DOWN); break;
	case ACTION_MOVE_LEFT:	try_move(player, actor, VEC2I_LEFT); break;
	case ACTION_MOVE_RIGHT:	try_move(player, actor, VEC2I_RIGHT); break;
	case ACTION_NONE: break;
	}
}


void player_tick(Player *player, double dt) {
	if (!player->active) return;
	Actor *actor = actor_get(player->actor);
	gcd_tick(&actor->gcd, dt);
	if (!gcd_is_free(&actor->gcd))
		return;

	player_act(player, actor);
	player->action = ACTION_NONE;

	game_set_dirty();
}


static bool is_opaque(int x, int y, void *_data) {
	(void)(_data); // unused
	Tile tile = map_get(&game.map, x, y);
	return tile_is_opaque(tile);
}

static void on_visible(int x, int y, void *data) {
	Player *player = data;
	vision_set_visible(&player->vision, x, y, true);
	vision_set_revealed(&player->vision, x, y, true);
}

static void update_player_fov(Player *player, Vec2i position) {
	vision_clear_visible(&player->vision);
	fov_2d(
		position.x, position.y, 5,
		is_opaque, on_visible,
		player
	);
}
