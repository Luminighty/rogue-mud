#ifndef PLAYER_H
#define PLAYER_H


#include "actor.h"
#include "display.h"
#include "linalg.h"
#include "vision.h"
#include <stdbool.h>


typedef enum {
	ACTION_NONE,
	ACTION_MOVE_UP,
	ACTION_MOVE_DOWN,
	ACTION_MOVE_LEFT,
	ACTION_MOVE_RIGHT,
} PlayerAction;


typedef struct {
	bool active;
	PlayerAction action;
	ActorId actor;
	Vision vision;
} Player;


void player_init(Player *player, Vec2i position);
void player_tick(Player *player, double dt);
void player_render(Display *display, Player *player, bool is_current_player);
void player_set_action(Player *player, PlayerAction action);


#endif // PLAYER_H
