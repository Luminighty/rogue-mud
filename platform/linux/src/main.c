#include "client.h"
#include "config.h"
#include "deltatime.h"
#include "game.h"
#include "network.h"
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


static void handle_signal(int signal) {
	if (signal == SIGINT) {
		game_exit();
		return;
	}
	exit(1);
}


static inline void render_clients() {
	for (int i = 0; i < PLAYER_COUNT; i++) {
		Client *client = client_get(i);
		if (!client->connected)
			continue;
		game_render(&client->display, client->player_index);
		client_set_dirty(client);
	}
}


int main() {
	signal(SIGINT, handle_signal);

	delta_time_init();
	network_init();
	game_init();
	while (game_is_running()) {
		network_update();

		double delta_ms = delta_time_get();
		bool stepped = game_tick(delta_ms);
		if (stepped)
			render_clients();

		network_render_step();
		fflush(stdout);

		usleep(10 * 1000);
	}
	game_destroy();
	network_destroy();
	return 0;
}

