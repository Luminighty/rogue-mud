#include "game.h"
#include "client.h"
#include "config.h"
#include "display.h"
#include "gcd.h"
#include "actor.h"
#include "linalg.h"
#include "map.h"
#include "network.h"
#include "palette.h"
#include "player.h"
#include "prefabs.h"
#include <stdbool.h>


Game game = {0};

void game_init() {
	actors_init();

	game.running = true;
	map_generate(&game.map);
	player_init(&game.players[0], ((Vec2i){.x = 10, .y = 10}));
	player_init(&game.players[1], ((Vec2i){.x = 8, .y = 9}));

	goblin_create(((Vec2i){ .x=17, .y = 17}));
	orc_create(((Vec2i){    .x=5,  .y =  8}));
	potion_create(((Vec2i){ .x=4,  .y =  6}));
	scroll_create(((Vec2i){ .x=16, .y = 14}));
	goblin_create(((Vec2i){ .x=26, .y = 9}));
	goblin_create(((Vec2i){ .x=30, .y = 8}));
	goblin_create(((Vec2i){ .x=27, .y = 12}));
	orc_create(((Vec2i){ .x=24, .y = 15}));
	potion_create(((Vec2i){ .x=24,  .y = 24}));
	potion_create(((Vec2i){ .x=42,  .y = 8}));
}


void game_destroy() {

}


bool game_tick(double dt) {
	game.is_dirty = false;
	game.time += dt;

	for (int i = 0; i < PLAYER_COUNT; i++) {
		player_tick(&game.players[i], dt);
	}
	actors_tick(&game.actors, dt);

	return game.is_dirty;
}


void game_exit() {
	game.running = false;
}


void game_render(Display *display, int player_idx) {
	Player *player = &game.players[player_idx];
	display_clear(display);
	map_render(display, &game.map, &player->vision);
	actors_render(display, &game.actors, &player->vision);

}


void game_input(int player_idx, Key key) {
	Player *player = &game.players[player_idx];
	Actor *actor = actor_get(player->actor);
	if (gcd_remaining(&actor->gcd) > 100.0)
		return;
	switch (key) {
	case KEY_UP:	player_set_action(player, ACTION_MOVE_UP); break;
	case KEY_DOWN:	player_set_action(player, ACTION_MOVE_DOWN); break;
	case KEY_LEFT:	player_set_action(player, ACTION_MOVE_LEFT); break;
	case KEY_RIGHT:	player_set_action(player, ACTION_MOVE_RIGHT); break;
	case KEY_Q:	client_close(client_get(player_idx)); break;
	default: break;
	}

}


bool game_is_solid(int x, int y) {
	return map_is_solid(&game.map, x, y);
}

