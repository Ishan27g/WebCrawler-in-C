
#include "http_headers.h"

void* fill_http_header(char *dest_string, Http_header *dest)
{
	if(!dest_string || !dest){
		//fprintf("\nEmpty pointer passed\n");
	}
	char *key, *value, *tmp;
	char* tmp2 = NULL;
	key = strtok_r(dest_string, ":", &tmp);
	if(key == NULL)
	{
		return NULL;
	}

	if(strstr(key,"HTTP") != NULL)
	{
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

		if(strstr(key,"Server") !=NULL)
		{
			memset(dest->http_server,'\0',sizeof(dest->http_server));
			strcpy(dest->http_server, value);
		}
		else if(strstr(key,"Content-Type") !=NULL)
		{
			memset(dest->http_content_type,'\0',sizeof(dest->http_content_type));
			strcpy(dest->http_content_type, value);
			dest->http_content_type[strlen(dest->http_content_type)-1]='\0';
		}
		else if(strstr(key,"Content-Length") !=NULL)
		{
			dest->http_content_length = atoi(value);
		}
		else if(strstr(key,"Retry-After") !=NULL)
		{
			dest->http_retry_after = atoi(value);
		}
		else if(strstr(key,"Location") !=NULL)
		{
			memset(dest->http_location,'\0',sizeof(dest->http_location));
			strcpy(dest->http_location, value);
		}
		else if(strstr(key,"Location") !=NULL)
		{
			memset(dest->http_location,'\0',sizeof(dest->http_location));
			strcpy(dest->http_location, value);
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
	//	fprintf(stdout,"\nEmpty pointer passed\n");
	}
	char *dest_string, *dest_temp;
	char string_copy[1024];
	dest_string = strtok_r(buffer, "<", &dest_temp);
	sprintf(html_content, "<%s",dest_temp);
	dest_temp = NULL;
	dest_string = strtok_r(dest_string, "\n", &dest_temp);
	do {
		if(dest_string != NULL)
		{
			memset(string_copy, '\0', 1024);
			strcpy(string_copy, dest_string);
			fill_http_header(string_copy, dest);
		}
	} while ((dest_string = strtok_r(NULL, "\n", &dest_temp)) != NULL);
	return NULL;

}

