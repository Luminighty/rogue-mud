#include "game.h"
#include "client.h"
#include "config.h"
#include "display.h"
#include "gcd.h"
#include "actor.h"
#include "key.h"
#include "linalg.h"
#include "map.h"
#include "network.h"
#include "palette.h"
#include "player.h"
#include "prefabs.h"
#include "random.h"
#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

Game *game = NULL;

void game_init() {
	assert(game != NULL);
	memset(game, 0, sizeof(Game));

	random_init();
	actors_init();

	game->running = true;
	map_generate(&game->map);

	assert(game->map.room_c > 0);
	Room *first_room = &game->map.rooms[0];
	Vec2i spawn = rect2i_center(first_room->rect);
	player_init(&game->players[0], spawn, "Luminight", COLOR_YELLOW);
	player_init(&game->players[1], vec2i_add(spawn, VEC2I_RIGHT), "Alias_01", COLOR_TEAL);

	foreach_room(&game->map, i) {
		if (i == 0)
			continue;
		Vec2i center = rect2i_center(game->map.rooms[i].rect);
		if (random_range(0, 5) < 3) {
			goblin_create(center);
		} else {
			orc_create(center);
		}

	}

	// goblin_create(((Vec2i){ .x=17, .y = 17}));
	// orc_create(((Vec2i){    .x=5,  .y =  8}));
	// potion_create(((Vec2i){ .x=4,  .y =  6}));
	// scroll_create(((Vec2i){ .x=16, .y = 14}));
	// goblin_create(((Vec2i){ .x=26, .y = 9}));
	// goblin_create(((Vec2i){ .x=30, .y = 8}));
	// goblin_create(((Vec2i){ .x=27, .y = 12}));
	// orc_create(((Vec2i){ .x=24, .y = 15}));
	// potion_create(((Vec2i){ .x=24,  .y = 24}));
	// potion_create(((Vec2i){ .x=42,  .y = 8}));
}


void game_destroy() {

}


bool game_tick(double dt) {
	game->is_dirty = false;
	game->time += dt;

	for (int i = 0; i < PLAYER_COUNT; i++) {
		player_tick(&game->players[i], dt);
	}
	actors_tick(&game->actors, dt);

	return game->is_dirty;
}


void game_exit() {
	game->running = false;
}


void game_render(Display *display, int player_idx) {
	Player *player = &game->players[player_idx];
	Actor *actor = actor_get(player->actor);
	display_clear(display);
	map_render(display, &game->map, &player->vision);
	actors_render(display, &game->actors, &player->vision);

	static const int OFFSET_Y = MAP_HEIGHT;
	static const int HEIGHT = DISPLAY_HEIGHT - MAP_HEIGHT;

	display_box(display, 0, OFFSET_Y, 80, HEIGHT);

	// PLAYER LOGS
	for (int i = 0; i < PLAYER_LOG_SIZE; i++) {
		int log = (player->log.first_index + i) % PLAYER_LOG_SIZE;
		if (player->log.entry_length[log] == 0)
			break;
		display_string(
			display,
			1, OFFSET_Y + 1 + i, 0,
			player->log.logs[log], COLOR_WHITE, COLOR_BLACK
		);
	}

	// PLAYER NAME
	int x = 4;
	display_set(display, x++, OFFSET_Y, Z_SKIP, glyph(' ', COLOR_WHITE, COLOR_BLACK));
	x += display_string(display, x, OFFSET_Y, Z_SKIP, player->name, COLOR_WHITE, COLOR_BLACK);
	display_set(display, x++, OFFSET_Y, Z_SKIP, glyph(' ', COLOR_WHITE, COLOR_BLACK));
	x += 4;

	// HP
	x += display_string(display, x, OFFSET_Y, Z_SKIP, " HP: ", COLOR_YELLOW, COLOR_BLACK);
	x += display_int(display, x, OFFSET_Y, Z_SKIP, actor->hp, COLOR_YELLOW, COLOR_BLACK);
	x += display_string(display, x, OFFSET_Y, Z_SKIP, " / ", COLOR_YELLOW, COLOR_BLACK);
	x += display_int(display, x, OFFSET_Y, Z_SKIP, actor->hp, COLOR_YELLOW, COLOR_BLACK);
	display_set(display, x++, OFFSET_Y, Z_SKIP, glyph(' ', COLOR_YELLOW, COLOR_BLACK));
}


void game_input(int player_idx, KeyStroke key_stroke) {
	Player *player = &game->players[player_idx];
	Actor *actor = actor_get(player->actor);
	if (gcd_remaining(&actor->gcd) > 100.0)
		return;

	Vec2i dir = {0};
	PlayerActionKind kind = ACTION_NONE;

	// printf(KEYSTROKE_FMT"\n", KEYSTROKE_ARG(key_stroke));

	#define set_move(base, when_shift)\
		kind = ACTION_MOVE; base; if (key_stroke.shift) when_shift
	switch (key_stroke.key) {
	case KEY_UP:	set_move(dir.y = -1, dir.x = -1); break;
	case KEY_DOWN:	set_move(dir.y =  1, dir.x =  1); break;
	case KEY_LEFT:	set_move(dir.x = -1, dir.y =  1); break;
	case KEY_RIGHT:	set_move(dir.x =  1, dir.y = -1); break;
	case KEY_Q:
		client_close(client_get(player_idx));
		break;
	default: 
		printf("Unknown input: "KEYSTROKE_FMT"\n", KEYSTROKE_ARG(key_stroke));
		break;
	}
	if (kind == ACTION_NONE)
		return;
	player->action.kind = kind;
	player->action.dir = dir;
}


bool game_is_solid(int x, int y) {
	return map_is_solid(&game->map, x, y);
}

