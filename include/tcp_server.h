#ifndef TCP_SERVER_H
#define TCP_SERVER_H


#include <stddef.h>


void app_tcp_set_non_blocking(int socket);
int app_tcp_server_listen(int port, int listen_backlog);
int app_tcp_accept(int socket);
void app_tcp_close(int socket);
int app_tcp_recv(int socket, void *buffer, size_t length);
int app_tcp_send(int socket, const void *buffer, size_t length);


#endif // TCP_SERVER_H
