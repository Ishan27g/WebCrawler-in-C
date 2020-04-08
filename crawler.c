
#include <stdio.h>
#include <pthread.h>
#include "html_file_parser.h"
#include "crawler.h"
#include "socket_operations.h"
char* original_host;
char* resource;


int parse_input(char* input)
{
	char* components, *dest_temp;
	char* input_string;
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
		strncpy(input_string, input+7, strlen(input)-7);
		components = strtok_r(input_string, "/", &dest_temp);
		strcpy(original_host, components);
		strncpy(resource, dest_temp, strlen(dest_temp));
		free(input_string);
	}
	return 0;
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
	memset(original_host,'\0',len);
	resource = malloc(len);
	memset(resource,'\0',len);
	parse_input(argv[1]);
	resource[strlen(resource)]='\0';	
	fprintf( stderr,"\nhost is %s\n", original_host);
	fprintf( stderr,"\nresource is %s\n", resource);

	client_socket = initialise_socket();
	if(client_socket == 0)
	{
		fprintf( stderr,"\nsocket not initialised\n");
		return 0;
	}
	
	int head=0;	
	send_receive_socket_data(client_socket, resource, head);
  //      close(client_socket);

	Web_crawler crawler;
	int i=0;
	int href_count = 0;

		
	crawler.href_url_count = 0;

	for(i=0;i<100;i++)
	{	
		memset(crawler.href_url[i].resource_filename,'\0',sizeof(crawler.href_url[i].resource_filename));
		memset(crawler.href_url[i].hostname,'\0',sizeof(crawler.href_url[i].hostname));
		memset(crawler.href_url[i].local_file,'\0',sizeof(crawler.href_url[i].local_file));
		
		//use this somewhere ?????
		crawler.href_url[i].to_visit = false;
	}
//while this count is less than 100
	href_count = read_file(HTML_FILE_LOCAL, &crawler, head);
	//now file is useles, valid content copied to crawler_obj, can delete
	printf("\ninitial values read : %d\n",href_count);
//	int socket = initialise_socket();
	for(i=0;i<href_count;i++)
	{
		printf("\ncrawler.href_url[%d].resource_filename : %s",i,crawler.href_url[i].resource_filename);
		printf("\ncrawler.href_url[%d].hostname : %s\n",i,crawler.href_url[i].hostname);
		
//		send_receive_socket_data(socket, crawler.href_url[i].resource_filename, i);
		if(i==1)
			href_count = read_file(HTML_FILE_LOCAL, &crawler, head+1);
		printf("\nat i= %d, href_count is now %d\n",i,href_count);


		//send req_ recieve_file_num_x
		//read_file() , this will start crawler_obj at index = href_count
		//adding new url to next index
		//and updating href_count as well as for loop 
		//
		//handling for mime type- text/html
		//handling for response codes
		//
		//delete the files also, can add file pointer variable to crawler_obj
	}
	printf("\nfinal href_count %d\n",href_count);
	
	return 0;
}

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
