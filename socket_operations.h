#ifndef SOCK_H
#define SOCK_H

#include "crawler.h"
#include "http_headers.h"

#define MIME_TYPE_TEXT_HTML "text/html"
int send_receive_socket_data(int client_socket, char* resource, int auth_flag, int relative);
int initialise_socket(char* host);

#endif
