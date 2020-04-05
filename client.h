
#include <errno.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>


#define HTTP_REQ_STR_LEN strlen(HTTP_REQ_STR)
#define HTTP_REQ_STR "HTTP/1.1\r\nUser-Agent: igoyal\r\nHost: 172.26.129.100\r\nConnection: close\r\n\r\n"
#define PORT_NUMBER 80
#define HTML_FILE_LOCAL "local_html_file.html"
#define MAX_URL_STR_LEN 256

