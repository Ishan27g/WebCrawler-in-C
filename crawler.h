
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
#define HTTP_REQ_STR "HTTP/1.1\r\nUser-Agent: igoyal\r\nHost: 172.26.129.100\r\nConnection: keep-alive\r\n\r\n"
#ifdef VM_DEBUG_ON
	#define PORT_NUMBER 8080
#else
	#define PORT_NUMBER 80
#endif

#define HTML_FILE_LOCAL "local_html_file.html"
#define MAX_URL_STR_LEN 256
#define QUEUE_SIZE 256

typedef struct Href_url_tag{
	char resource_filename[1024];
	char hostname[32];
	char local_file[32];
	bool visited;
}Href_url;
typedef struct Web_crawler_struct{
	Href_url href_url[100];
	int href_url_count;
	int visited_count;
}Web_crawler;

#endif //CRAWLR_H
 


