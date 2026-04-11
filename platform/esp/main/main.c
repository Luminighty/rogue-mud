#include "deltatime.h"
#include "esp_err.h"
#include "esp_log_level.h"
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "game.h"
#include "lwip/sockets.h"
#include "network.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "sdkconfig.h"

#include "wifi_setup.h"
#include "tcp_server.h"
#include <stdio.h>

static const char *TAG = "ESPMAIN";


static inline void render_clients() {
	for (int i = 0; i < PLAYER_COUNT; i++) {
		Client *client = client_get(i);
		if (!client->connected)
			continue;
		game_render(&client->display, client->player_index);
		client_set_dirty(client);
	}
}


void app_main(void) {
	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);

	if (CONFIG_LOG_MAXIMUM_LEVEL > CONFIG_LOG_DEFAULT_LEVEL) {
		esp_log_level_set("wifi", CONFIG_LOG_MAXIMUM_LEVEL);
	}
	ESP_LOGI(TAG, "wifi_init_sta(): START");
	wifi_setup();
	ESP_LOGI(TAG, "wifi_init_sta(): DONE");

	// xTaskCreate(tcp_server_task, "tcp_server", 4096, (void*)AF_INET, 5, NULL);

	delta_time_init();
	network_init();
	game_init();
	ESP_LOGI(TAG, "Game start");
	while (game_is_running()) {
		network_update();

		double delta_ms = delta_time_get();
		bool stepped = game_tick(delta_ms);
		if (stepped)
			render_clients();
		network_render_step();
		fflush(stdout);
		vTaskDelay(pdMS_TO_TICKS(10));
	}
	ESP_LOGI(TAG, "Exiting.");
	game_destroy();
	network_destroy();
}

