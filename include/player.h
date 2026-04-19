#ifndef PLAYER_H
#define PLAYER_H


#include "actor_id.h"
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

#define PLAYER_NAME_SIZE 16

#define PLAYER_LOG_SIZE (DISPLAY_HEIGHT - MAP_HEIGHT - 2)
#define PLAYER_LOG_ENTRY_LEN 78

typedef struct {
	// NOTE: Plus one for \0
	char logs[PLAYER_LOG_SIZE][PLAYER_LOG_ENTRY_LEN + 1];
	int entry_length[PLAYER_LOG_SIZE];
	int first_index;
} PlayerLog;

typedef struct {
	bool active;
	PlayerAction action;
	ActorId actor;
	Vision vision;
	char name[PLAYER_NAME_SIZE];
	PlayerLog log;
} Player;


void player_init(Player *player, Vec2i position, char *name);
void player_tick(Player *player, double dt);
void player_render(Display *display, Player *player, bool is_current_player);
void player_set_action(Player *player, PlayerAction action);

void player_log_begin(PlayerLog *logs);
void player_log_push(PlayerLog *logs, const char *log);


#endif // PLAYER_H
