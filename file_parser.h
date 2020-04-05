
#ifndef FILE_PARSER_H
#define FILE_PARSER_H
#include "crawler.h"
typedef struct Href_url_tag{
	char* resource_filename;
	char* hostname;
	bool to_visit;
}Href_url;

void parse_html_file(char* filename, Web_crawler *crawler);
#endif
