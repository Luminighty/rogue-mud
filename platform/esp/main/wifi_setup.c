#include "wifi_setup.h"

#include "esp_bit_defs.h"
#include "esp_err.h"
#include "esp_event_base.h"
#include "esp_netif_ip_addr.h"
#include "esp_netif_types.h"
#include "esp_wifi_types_generic.h"
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"

#include "portmacro.h"
#include "sdkconfig.h"


#define ESP_MAXIMUM_RETRY 10
#define WIFI_FAIL_BIT BIT1
#define WIFI_CONNECTED_BIT BIT0

#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_PSK
#define ESP_WIFI_SAE_MODE WPA3_SAE_PWE_HUNT_AND_PECK
#define H2E_IDENTIFIER ""

static EventGroupHandle_t event_group_wifi;
static int retries = 0;
static const char *TAG = "WIFI";


static void event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
	if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
		esp_wifi_connect();
	} else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
		if (retries < ESP_MAXIMUM_RETRY) {
			esp_wifi_connect();
			retries++;
			ESP_LOGI(TAG, "Disconnected, retrying... ( %d / %d )", retries, ESP_MAXIMUM_RETRY);
		} else {
			xEventGroupSetBits(event_group_wifi, WIFI_FAIL_BIT);
		}
	} else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
		ip_event_got_ip_t *event = event_data;
		ESP_LOGI(TAG, "Got ip: " IPSTR, IP2STR(&event->ip_info.ip));
		retries = 0;
		xEventGroupSetBits(event_group_wifi, WIFI_CONNECTED_BIT);
	}
}


void wifi_setup(void) {
	event_group_wifi = xEventGroupCreate();

	ESP_ERROR_CHECK(esp_netif_init());
	ESP_ERROR_CHECK(esp_event_loop_create_default());
	esp_netif_create_default_wifi_sta();

	wifi_init_config_t init_config = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&init_config));

	esp_event_handler_instance_t instance_any_id;
	esp_event_handler_instance_t instance_got_ip;
	ESP_ERROR_CHECK(
		esp_event_handler_instance_register(
			WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, &instance_any_id
		)
	);
	ESP_ERROR_CHECK(
		esp_event_handler_instance_register(
			IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL, &instance_got_ip
		)
	);

	ESP_LOGI(TAG, "Attempting to join wifi with config: SSID: %s, Password: %s", CONFIG_ESP_WIFI_SSID, CONFIG_ESP_WIFI_PASSWORD);
	wifi_config_t wifi_config = {
		.sta = {
			.ssid = CONFIG_ESP_WIFI_SSID,
			.password = CONFIG_ESP_WIFI_PASSWORD,
			.threshold.authmode = ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD,
			.sae_pwe_h2e = ESP_WIFI_SAE_MODE,
			.sae_h2e_identifier = H2E_IDENTIFIER
		}
	};
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
	ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
	ESP_ERROR_CHECK(esp_wifi_start());

	ESP_LOGI(TAG, "wifi_init_sta done");

	EventBits_t bits = xEventGroupWaitBits(
		event_group_wifi,
		WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
		pdFALSE, pdFALSE,
		portMAX_DELAY
	);

	if (bits & WIFI_CONNECTED_BIT) {
		ESP_LOGI(TAG, "Connected to SSID: %s, password: %s", CONFIG_ESP_WIFI_SSID, CONFIG_ESP_WIFI_PASSWORD);
	} else if (bits & WIFI_FAIL_BIT) {
		ESP_LOGI(TAG, "Failed to connect to SSID: %s, password: %s", CONFIG_ESP_WIFI_SSID, CONFIG_ESP_WIFI_PASSWORD);
	} else {
		ESP_LOGE(TAG, "Unexpected Event!");
	}
}

