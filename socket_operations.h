#ifndef SOCK_H
#define SOCK_H

#include "crawler.h"
#include "http_headers.h"
void* send_receive_socket_data(int client_socket, char* resource, int count);
int initialise_socket();

#endif
