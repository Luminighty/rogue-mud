#ifndef GAME_H
#define GAME_H

#include "config.h"
#include "actor.h"
#include "key.h"
#include "map.h"
#include "player.h"
#include <stdbool.h>


typedef struct {
	double time;
	bool running;
	Map map;
	Player players[PLAYER_COUNT];
	Actors actors;

	bool is_dirty;
} Game;


extern Game game;

void game_init();
void game_destroy();
bool game_tick(double dt);
void game_render(Display *display, int player);
void game_input(int player, Key key);
bool game_is_solid(int x, int y);
void game_exit();


static inline bool game_is_running() {
	return game.running;
}
static inline bool game_should_update() {
	return game.running;
}
static inline void game_set_dirty() {
	game.is_dirty = true;
}


#endif // GAME_H
