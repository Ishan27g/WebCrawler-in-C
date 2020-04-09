
#ifndef HTTP_HDRS_H
#define HTTP_HDRS_H
#include "crawler.h"
typedef struct Http_rsp_headers{
	char http_version[50];
	char http_rsp_code[50];
	char http_server[512];
	char http_content_type[50];
	int http_content_length;
	int http_retry_after;
} Http_header;

enum http_rsp_code{
	HTTP_RSP_504_GW_TIMEOUT,
	HTTP_RSP_503_SERVICE_UNAVAILABLE,
	HTTP_RSP_414_REQ_URI_LONG,
	HTTP_RSP_410_PERM_GONE,
	HTTP_RSP_404_NOT_FOUND,
	HTTP_RSP_200_SUCCESS
};

void* get_http_header(char* buffer, Http_header *dest, char* html_content);
void* fill_http_header(char *dest_string, Http_header *dest);

#endif
