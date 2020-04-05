
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
		strcpy(input_string, input+7);
		components = strtok_r(input_string, "/", &dest_temp);
		strcpy(host, components);
		strcpy(resource, dest_temp);
		return host;//components;
	}
	return NULL;
}

int main(int argc, char **argv)
{	
	int client_socket;
	if(!argv[1])
	{
		printf("\ninvalid input string\n");
		return 0;
	}
	original_host = NULL;
	original_host = (char*) calloc(strlen(argv[1]), sizeof(char));
	resource = malloc(strlen(argv[1]));
	original_host = parse_input(argv[1], resource);	
	printf("\nhost is %s", original_host);
	printf("\nresource is %s", resource);

	Web_crawler crawler;

	client_socket = initialise_socket();
	
	send_receive_socket_data(client_socket, resource);

        close(client_socket);
	
	crawler.visited_count = 0;
	int i=0;
	for(i=0;i<100;i++)
	{
		crawler.visit[i] = 0;
	}
	parse_html_file(HTML_FILE_LOCAL, &crawler);
	for(i=0;i<1;i++)
	{
		printf("\nsend request for %s\n",crawler.visited_url[i]);
		//add following as task to the threadpool
		//	send req

		client_socket = initialise_socket();
		send_receive_socket_data(client_socket,crawler.visited_url[i]);
		close(client_socket);
		//	on valid response, mark crawler.visit[i] = 1
		//	create file
		//	parse valid urls
		//	add urls to crawler
	}


	printf("\nUrls visited - %d\n",crawler.visited_count);
	
	return 0;
}
