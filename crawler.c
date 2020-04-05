
#include <stdio.h>
#include <pthread.h>
#include "crawler.h"
#include "file_parser.h"
#include "socket_operations.h"
char* original_host;


char* parse_input(char* input, char* resource)
{
	char *components, *dest_temp;
	char *host;
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
		//strcpy(input_string, host+7);
		components = strtok_r(input+7, "/", &dest_temp);
		strcpy(host, components);
		strcpy(resource, components);
		return host;//components;
	}
	return NULL;
}

int main(int argc, char **argv)
{	
	int client_socket;
	char *resource;
	char *host;
	if(argv[1])
	{
		host = malloc(strlen(argv[1]));
		resource = malloc(strlen(argv[1]));
	}
	else
	{
		printf("\ninvalid input string\n");
		return NULL;
	}

	host = parse_input(argv[1], resource);	
	printf("\nhost is %s", host);
	printf("\nresource is %s", resource);

	Web_crawler crawler;

	original_host = NULL;

	if(argv[1] == NULL)
	{
		printf("\nEnter hostname as 1st arguement\n");
	}
	original_host = (char*) calloc(strlen(argv[1]), sizeof(char));
	strcpy(original_host, argv[1]);
	printf("\nhost is %s\n",original_host);

	client_socket = initialise_socket();
	
	send_receive_socket_data(client_socket, NULL);

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
