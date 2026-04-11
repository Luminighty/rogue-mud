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
#include <fcntl.h>


static const char *TAG = "TCP_SERVER";
static int keep_alive = 1;
static int keep_idle = CONFIG_TCP_KEEP_ALIVE_IDLE;
static int keep_interval = CONFIG_TCP_KEEP_ALIVE_INTERVAL;
static int keep_count = CONFIG_TCP_KEEP_ALIVE_COUNT;


static void tcp_server_cleanup(int socket) {
	app_tcp_close(socket);
}

void app_tcp_set_non_blocking(int socket) {
	int flags = fcntl(socket, F_GETFL, 0);
	fcntl(socket, F_SETFL, flags | O_NONBLOCK);
}


int app_tcp_server_listen(int port, int listen_backlog) {
	struct sockaddr_in dest_addr = {0};
	dest_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(port);

	int server = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (server < 0) {
		ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
		return -1;
	}
	int opt = 1;
	setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	app_tcp_set_non_blocking(server);

	ESP_LOGI(TAG, "Socket created");
	int err;
	err = bind(server, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
	if (err != 0) {
		ESP_LOGE(TAG, "Socket unable to bind. errno %d", errno);
		tcp_server_cleanup(server);
		return -1;
	}
	ESP_LOGI(TAG, "Socket bound. Port %d", port);
	err = listen(server, 1);
	if (err != 0) {
		ESP_LOGE(TAG, "Error occurred during 'listen'. errno %d", errno);
		tcp_server_cleanup(server);
		return -1;
	}
	return server;
}


int app_tcp_accept(int socket) {
	int client = accept(socket, NULL, NULL);
	if (client < 0) {
		if (errno != EWOULDBLOCK && errno != EAGAIN)
			ESP_LOGE(TAG, "Unable to accept connection: errno %d", errno);
		return -1;
	}
	setsockopt(client, SOL_SOCKET, SO_KEEPALIVE, &keep_alive, sizeof(int));
	setsockopt(client, IPPROTO_TCP, TCP_KEEPIDLE, &keep_idle, sizeof(int));
	setsockopt(client, IPPROTO_TCP, TCP_KEEPINTVL, &keep_interval, sizeof(int));
	setsockopt(client, IPPROTO_TCP, TCP_KEEPCNT, &keep_count, sizeof(int));
	ESP_LOGI(TAG, "Socket accepted %d", client);
	return client;
}


void app_tcp_close(int socket) {
	close(socket);
}


int app_tcp_recv(int socket, void *buffer, size_t length) {
	return recv(socket, buffer, length, 0);
}


int app_tcp_send(int socket, const void *buffer, size_t length) {
	return send(socket, buffer, length, MSG_NOSIGNAL);
}

