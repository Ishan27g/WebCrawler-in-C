
#ifndef CRAWLER_H
#define CRAWLER_H


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
#define HTTP_REQ_STR "HTTP/1.1\r\nUser-Agent: igoyal\r\nHost: igoyal_c30023\r\nConnection: close\r\n\r\n"
#define HTTP_AUTH_STR "HTTP/1.1\r\nAuthorization: Basic \r\nUser-Agent: igoyal\r\nHost: igoyal_c30023\r\nConnection: close\r\n\r\n"
//#define HTTP_REQ_STR "HTTP/1.1\r\nUser-Agent: igoyal\r\nHost: 172.26.129.100\r\nConnection: keep-alive\r\n\r\n"
#ifdef VM_DEBUG_ON
	#define PORT_NUMBER 8080
#else
	#define PORT_NUMBER 80
#endif

#define HTML_FILE_LOCAL "local.html"
#define MAX_URL_STR_LEN 256
#define QUEUE_SIZE 256

typedef struct Href_url_tag{
	char resource_filename[1024];
	char hostname[32];
	int relative;
}Href_url;

typedef struct Web_crawler_struct{
	Href_url href_url[100];
	int href_url_count;
	int visited_count;
}Web_crawler;

#endif //CRAWLR_H
 


