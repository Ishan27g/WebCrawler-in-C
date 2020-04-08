
#include <stdio.h>
#include <pthread.h>
#include "html_file_parser.h"
#include "crawler.h"
#include "socket_operations.h"
char* original_host;
char* resource;

Web_crawler crawler;

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

	client_socket = initialise_socket("");
	if(client_socket == 0)
	{
		fprintf( stderr,"\nsocket not initialised\n");
		return 0;
	}
	
	int ret = send_receive_socket_data(client_socket, resource);
        close(client_socket);
	fprintf(stderr,"\n*******____________*********___________*******_______________\n");
	if(ret != 1)
	{
		fprintf(stderr,"\nCONTENT IS NOT TEXT/HTML\n");
		return 0;
	}
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
	//need to add the first url to crawler_obj
	strcpy(crawler.href_url[0].resource_filename, resource); 
	strcpy(crawler.href_url[0].hostname, original_host); 
	crawler.href_url_count++;

//while this count is less than 100
	href_count = read_file(HTML_FILE_LOCAL);
	int visited_count = crawler.href_url_count;
	//now file is useles, valid content copied to crawler_obj, can delete
	fprintf(stderr,"\ninitial values read : %d\n",href_count);
	fprintf(stderr,"\ncrawler.href_url[%d].resource_filename : %s",0,crawler.href_url[0].resource_filename);
	fprintf(stderr,"\ncrawler.href_url[%d].hostname : %s\n",0,crawler.href_url[0].hostname);
	fprintf(stderr,"\nNow crawling following links:------------------");
//	int socket = initialise_socket();
	for(i=1;i<href_count;i++)
	{
		printf("\ncrawler.href_url[%d].resource_filename : %s",i,crawler.href_url[i].resource_filename);
		printf("\ncrawler.href_url[%d].hostname : %s\n",i,crawler.href_url[i].hostname);
			
		client_socket = initialise_socket(crawler.href_url[i].hostname);
		ret = 0;
		ret = send_receive_socket_data(client_socket, crawler.href_url[i].resource_filename);
		close(client_socket);

		if(ret == 1)
		{
			href_count = read_file(HTML_FILE_LOCAL);
			visited_count ++;
		//adding next valid url to consequent index of crawler
		//and updated href_count as well as for loop 
		//
		//handling for response codes
		//
		//delete the files also, can add file pointer variable to crawler_obj
		}
		else
		{
			fprintf(stderr,"\ncontent type is not text/html\n");
		}
		fprintf(stderr,"\n*******____________*********___________*******_______________\n");
		fprintf(stderr,"\n*******____________*********___________*******_______________\n");
	}
	printf("\nfinal href_count %d\n",href_count);
	
	return 0;
}

