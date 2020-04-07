
#include <stdio.h>
#include <pthread.h>
#include "crawler.h"
#include "file_parser.h"
#include "socket_operations.h"
char* original_host;
char *resource;


char* parse_input(char* input, char* resource)
{
	char *components, *dest_temp;
	char *host;
	char *input_string;
	host = strstr(input,"http://");
	if(host == NULL)
	{
		host = strstr(input,"HTTP://");
	}
	if(host == NULL)
	{
		//string is in format name.resource

	}
	else
	{
		//string is in http://name.resource format
		//char* host not points to name
		input_string = malloc(strlen(input)-7);
		strncpy(input_string, input+7, strlen(input)-7);
		components = strtok_r(input_string, "/", &dest_temp);
		strncpy(host, components, strlen(components));
		strncpy(resource, dest_temp, strlen(dest_temp));;
		free(input_string);
		return host;//components;
	}
	return NULL;
}

int main(int argc, char **argv)
{	
	int client_socket;
	if(!argv[1])
	{
		fprintf( stderr,"\ninvalid input string\n");
		return 0;
	}
	int len = strlen(argv[1]);
	original_host = NULL;
	original_host = malloc(len);
	resource = malloc(len);
	original_host = parse_input(argv[1], resource);	
	fprintf( stderr,"\nhost is %s\n", original_host);
	fprintf( stderr,"\nresource is %s\n", resource);
	Web_crawler crawler;

	client_socket = initialise_socket();
	if(client_socket == 0)
	{
		fprintf( stderr,"\nsocket not initialised\n");
		return 0;
	}
	
	send_receive_socket_data(client_socket, resource);
        close(client_socket);



#if 0
	crawler.visited_count = 0;
	int i=0;
	for(i=0;i<100;i++)
	{
		crawler.visit[i] = 0;
		crawler.visited_url[i] = NULL;
	}

	parse_html_file(HTML_FILE_LOCAL, &crawler);

	for(i=0;i< crawler.visited_count;i++)
	{
		fprintf( stderr,"\nTo send request for %s\n",crawler.visited_url[i]);
		//add following as task to the threadpool
		//	send req
/*
		client_socket = initialise_socket();
		send_receive_socket_data(client_socket,crawler.visited_url[i]);
		close(client_socket);
*/
		//	on valid response, mark crawler.visit[i] = 1
		//	create file
		//	parse valid urls
		//	add urls to crawler
	}

	fprintf( stderr,"\nUrls visited - %d\n",crawler.visited_count);
	free(original_host);
	free(resource);

	return 0;
}
#endif
