
#ifndef HTTP_HDRS_H
#define HTTP_HDRS_H
#include "crawler.h"
typedef struct Http_rsp_headers{
	char* http_version;
	char* http_rsp_code;
	char* http_server;
	char* http_content_type;
	int http_content_length;
} Http_header;



void* get_http_header(char* buffer, Http_header *dest, char* html_content);
void* fill_http_header(char *dest_string, Http_header *dest);

#endif
