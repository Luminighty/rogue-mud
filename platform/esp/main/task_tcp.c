#include "network.h"
#include "tcp_server.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "esp_event.h"
#include "esp_netif.h"

#include "lwip/err.h"
#include "lwip/inet.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>
#include <string.h>

static const char *TAG = "TCP_SERVER";


void task_tcp_server(void *pv_parameters) {

	// network_init();
	//
	// int server_socket = tcp_server_listen(1212, 10);
	// if (server_socket < 0) {
	// 	ESP_LOGE(TAG, "Failed to initialize server. errno: %d", errno);
	// 	vTaskDelete(NULL);
	// 	return;
	// }
}

