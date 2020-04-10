#include <stdio.h>
#include <pthread.h>
#include "html_file_parser.h"
#include "crawler.h"
#include "socket_operations.h"
char* original_host;
char* resource;
Web_crawler crawler;
Http_extension http_extension;
int parse_input(char* input)
{
	char* components, *dest_temp;
	char* input_string;
	input[strlen(input)]='\0';
	char* http_present = strstr(input,"http://");
	if(http_present == NULL)
	{
		http_present = strstr(input,"HTTP://");
	}
	if(http_present == NULL)
	{
		//hostname starts without http
		input_string = malloc(strlen(input));
		strcpy(input_string, input);
		components = strtok_r(input_string, "/", &dest_temp);
		strcpy(original_host, components);
		strcpy(original_host, components);
		strcpy(resource, dest_temp);
		free(input_string);
	}
	else
	{
		//string is in http://name.resource format
		//char* host not points to name
		input_string = malloc(strlen(input)-7);
		//strncpy(input_string, input+7, strlen(input)-7);
		strcpy(input_string, input+7);
		components = strtok_r(input_string, "/", &dest_temp);
		strcpy(original_host, components);
		strcpy(resource, dest_temp);
		free(input_string);
	}
	return 0;
}

int main(int argc, char **argv)
{	
	int client_socket;
	if(!argv[1])
	{
		return 0;
	}
	original_host = NULL;
	original_host = malloc(strlen(argv[1]));
	memset(original_host,'\0',strlen(argv[1]));
	resource = malloc(strlen(argv[1]));

	memset(resource,'\0',strlen(argv[1]));
	parse_input(argv[1]);
	resource[strlen(resource)]='\0';	
	if( resource[strlen(resource) - 2] == ' ')
	{
		resource[strlen(resource)- 2]='\0';
	}
	client_socket = initialise_socket("");
	if(client_socket == 0)
	{
		free(original_host);
		free(resource);
		return 0;
	}
	
	/*send request and recieve valid text/html resource*/
	int ret = send_receive_socket_data(client_socket, resource, HTTP_REQ_FLAG, 0);
        close(client_socket);
	if(ret == HTTP_RSP_504_GW_TIMEOUT)
	{
		sleep(1);
		client_socket = initialise_socket("");
		if(client_socket == 0)
		{
			free(original_host);
			free(resource);
			return 0;
		}
		/*send request again and recieve valid text/html resource*/
		ret = send_receive_socket_data(client_socket, resource, HTTP_REQ_FLAG, 0);
		close(client_socket);
		if(ret != 1)
		{
			free(original_host);
			free(resource);
			return 0;
		}
	}

	if(ret == HTTP_RSP_401_NOT_AUTH)
	{
		sleep(1);
		client_socket = initialise_socket("");
		if(client_socket == 0)
		{
			free(original_host);
			free(resource);
			return 0;
		}
		/*send request again and recieve valid text/html resource*/
		ret = send_receive_socket_data(client_socket, resource, HTTP_RSP_401_NOT_AUTH, 0);
		close(client_socket);
		if(ret != 1)
		{
			free(original_host);
			free(resource);
			return 0;
		}
	}
	if(ret == HTTP_RSP_301_MOVED_PERM)
	{
		sleep(1);
		client_socket = initialise_socket("");
		if(client_socket == 0)
		{
			free(original_host);
			free(resource);
			return 0;
		}
		/*send request again and recieve valid text/html resource*/
		ret = send_receive_socket_data(client_socket, http_extension.http_location, HTTP_REQ_FLAG, 0);
		close(client_socket);
		if(ret != 1)
		{
			free(original_host);
			free(resource);
			return 0;
		}
	}
	int i=0;
	int href_count = 0;

	crawler.href_url_count = 0;
	for(i=0;i<100;i++)
	{	
		memset(crawler.href_url[i].resource_filename,'\0',sizeof(crawler.href_url[i].resource_filename));
		memset(crawler.href_url[i].hostname,'\0',sizeof(crawler.href_url[i].hostname));
		//use this somewhere ?????
		crawler.href_url[i].relative = 0;
	}
	//need to add the first url to crawler_obj
	strcpy(crawler.href_url[0].resource_filename, resource); 
	strcpy(crawler.href_url[0].hostname, original_host); 
	crawler.href_url[0].relative = 0;
	crawler.href_url_count++;
	
	href_count = read_file(HTML_FILE_LOCAL);
	for(i=1;i<href_count;i++)
	{
		fprintf(stdout,"\n%s/%s",crawler.href_url[i].hostname,crawler.href_url[i].resource_filename);	
		sleep(1);
		client_socket = initialise_socket(crawler.href_url[i].hostname);
		ret = 0;
		ret = send_receive_socket_data(client_socket, crawler.href_url[i].resource_filename, HTTP_REQ_FLAG, crawler.href_url[i].relative);
		close(client_socket);
		if( ret == HTTP_RSP_504_GW_TIMEOUT)
		{
			sleep(1);
			client_socket = initialise_socket(crawler.href_url[i].hostname);
			ret = 0;
			ret = send_receive_socket_data(client_socket, crawler.href_url[i].resource_filename, HTTP_REQ_FLAG, crawler.href_url[i].relative);
			close(client_socket);
		}

		if(ret == 1)
		{
			href_count = read_file(HTML_FILE_LOCAL);
		}
		else if(ret == HTTP_RSP_401_NOT_AUTH)
		{
			sleep(1);
			client_socket = initialise_socket(crawler.href_url[i].hostname);
			ret = 0;
			ret = send_receive_socket_data(client_socket, crawler.href_url[i].resource_filename, HTTP_RSP_401_NOT_AUTH, crawler.href_url[i].relative);
			close(client_socket);
		}
		else if(ret == HTTP_RSP_301_MOVED_PERM)
		{
			sleep(1);
			client_socket = initialise_socket(crawler.href_url[i].hostname);
			ret = 0;
			ret = send_receive_socket_data(client_socket, http_extension.http_location, HTTP_REQ_FLAG, 0);
			close(client_socket);
		}
	}
	return 0;
}

