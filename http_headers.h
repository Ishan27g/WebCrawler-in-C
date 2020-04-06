
#ifndef HTTP_HDRS_H
#define HTTP_HDRS_H
#include "crawler.h"
typedef struct Http_rsp_headers{
	char http_version[10];
	char http_rsp_code[10];
	char http_server[512];
	char http_content_type[50];
	int http_content_length;
} Http_header;



void* get_http_header(char* buffer, Http_header *dest, char* html_content);
void* fill_http_header(char *dest_string, Http_header *dest);

#endif
