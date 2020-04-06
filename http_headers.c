
#include "http_headers.h"

void* fill_http_header(char *dest_string, Http_header *dest)
{
	if(!dest_string || !dest){
		printf("\nEmpty pointer passed\n");
	}
	char *key, *value, *tmp;
	char* tmp2 = NULL;
	int line_length = 0;
	key = strtok_r(dest_string, ":", &tmp);
	if(key == NULL)
	{
		return NULL;
	}
	line_length = strlen(key);

	if(strstr(key,"HTTP") != NULL)
	{
		//dest->http_version = malloc(4);
		memset(dest->http_version,'\0',sizeof(dest->http_version));
		memset(dest->http_rsp_code,'\0',sizeof(dest->http_version));
		strcpy(dest->http_version,strtok_r(key, " ", &tmp2));
		strcpy(dest->http_rsp_code, tmp2);
	}
	else
	{
		if(strlen(key) < 2)
		{
			return NULL;	
		}
		value = strtok_r(NULL, ":", &tmp);
		line_length = strlen(value);

		if(strstr(key,"Server") !=NULL)
		{
			memset(dest->http_server,'\0',sizeof(dest->http_server));
			strcpy(dest->http_server, value);
		}
		else if(strstr(key,"Content-type") !=NULL)
		{
			memset(dest->http_content_type,'\0',sizeof(dest->http_content_type));
			strcpy(dest->http_content_type, value);
		}
		else if(strstr(key,"Content-Length") !=NULL)
		{
			dest->http_content_length = atoi(value);
		}
		else
		{
			return NULL;
		}
	}
	return NULL;
}




void* get_http_header(char* buffer, Http_header *dest, char* html_content)
{
	if(!buffer || !dest){
		printf("\nEmpty pointer passed\n");
	}
	char *dest_string, *dest_temp;

	dest_string = strtok_r(buffer, "<", &dest_temp);
	//printf("\nhttp header is %s\n",dest_string);
	sprintf(html_content, "<%s",dest_temp);
	dest_temp = NULL;
	dest_string = strtok_r(dest_string, "\n", &dest_temp);
	do {
		if(dest_string != NULL)
		{
			fill_http_header(dest_string, dest);
		}
	} while ((dest_string = strtok_r(NULL, "\n", &dest_temp)) != NULL);
	return NULL;

}

