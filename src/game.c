#include "game.h"
#include "config.h"
#include "display.h"
#include "gcd.h"
#include "goblin.h"
#include "map.h"
#include "player.h"
#include <stdbool.h>
#include <stdio.h>


Game game = {0};


void game_init() {
	game.running = true;
	map_generate(&game.map);
	player_init(&game.players[0], 10, 10);
	player_init(&game.players[1], 8, 9);

	goblin_init(17, 17);
}


void game_destroy() {

}


bool game_tick(double dt) {
	game.is_dirty = false;
	game.time += dt;

	for (int i = 0; i < PLAYER_COUNT; i++) {
		player_tick(&game.players[i], dt);
	}
	for (int i = 0; i < 12; i++) {
		goblin_tick(&game.goblins[i], dt);
	}

	return game.is_dirty;
}


void game_exit() {
	game.running = false;
}


void game_render(Display *display, int player) {
	display_clear(display);
	map_render(display, &game.map);
	for (int i = 0; i < PLAYER_COUNT; i++) {
		player_render(display, &game.players[i], i == player);
	}
	for (int i = 0; i < 12; i++) {
		goblin_render(display, &game.goblins[i]);
	}
}


void game_input(int player_idx, Key key) {
	// printf("KEY: %d\n", key);
	Player *player = &game.players[player_idx];
	if (gcd_remaining(&player->gcd) > 100.0)
		return;
	switch (key) {
	case KEY_UP:	player_set_action(player, ACTION_MOVE_UP); break;
	case KEY_DOWN:	player_set_action(player, ACTION_MOVE_DOWN); break;
	case KEY_LEFT:	player_set_action(player, ACTION_MOVE_LEFT); break;
	case KEY_RIGHT:	player_set_action(player, ACTION_MOVE_RIGHT); break;
	default: break;
	}

}


bool game_is_solid(int x, int y) {
	return map_is_solid(&game.map, x, y);
}
