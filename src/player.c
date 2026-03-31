#include "player.h"
#include "display.h"
#include "game.h"
#include "gcd.h"
#include "glyph.h"
#include "palette.h"
#include <stdio.h>


void player_init(Player *player, int x, int y) {
	player->active = true;
	player->x = x;
	player->y = y;
	player->hp = 10;
}


void player_render(Display *display, Player *player, bool is_current_player) {
	if (!player->active)
		return;
	display_set(
		display, 
		player->x, player->y, 10, 
		glyph(
			'@',
			is_current_player ? COLOR_GREEN : COLOR_DARK_GREEN,
			COLOR_BLACK
		)
	);
}


void player_set_action(Player *player, PlayerAction action) {
	player->action = action;
}



static const double GCD_MOVE = 0.1 * 1000;


static inline void try_move(Player *player, int dx, int dy) {
	int new_x = player->x + dx;
	int new_y = player->y + dy;
	if (game_is_solid(new_x, new_y))
		return;
	player->x = new_x;
	player->y = new_y;
	gcd_set(&player->gcd, GCD_MOVE);
}

static inline void player_act(Player *player) {
	switch (player->action) {
	case ACTION_MOVE_UP:	try_move(player, 0, -1); break;
	case ACTION_MOVE_DOWN:	try_move(player, 0,  1); break;
	case ACTION_MOVE_LEFT:	try_move(player, -1, 0); break;
	case ACTION_MOVE_RIGHT:	try_move(player,  1, 0); break;
	case ACTION_NONE: break;
	}

}


void player_tick(Player *player, double dt) {
	gcd_tick(&player->gcd, dt);
	if (!gcd_is_free(&player->gcd))
		return;

	player_act(player);
	player->action = ACTION_NONE;

	game_set_dirty();
}

