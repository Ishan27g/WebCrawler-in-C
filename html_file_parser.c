

#include <errno.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

typedef struct Href_url_tag{
	char resource_filename[512];
	char hostname[32];
	char local_file[32];
	bool to_visit;
}Href_url;

typedef struct Web_crawler_struct{
	Href_url href_url[100];
	int href_url_count;
	int visited_count;
}Web_crawler;

void* free_ptr(void* ptr)
{
	if(ptr)
		free(ptr);
	ptr = NULL;
	return NULL;
}

int match_host(char* href_host)
{
	if(href_host == NULL)
	{
		printf("\nhost is null, not present in line, dont add the crawler_obj->href_element\n");
		return 0;
	}
	char* href_copy = (char*)malloc(strlen(href_host));
	char* hostname_copy = (char*)malloc(strlen("web1.comp30023"));

	strcpy(href_copy, href_host);
	strcpy(hostname_copy,"web1.comp30023");
	//printf("\ncomparing [%s] with [%s]\n",href_copy, hostname_copy);
	char* ho = strstr(hostname_copy,".");
	char* hr = strstr(href_copy,".");
	if (strcmp(ho, hr) == 0)
		return 1;
	else
		return 0;
	return 0;

}
int count_dots(char* string)
{
	char *components, *dest_temp;
	int count=0;
	components = strtok_r(string, ".", &dest_temp);
	if(components == NULL) 
	{
		return count;
	}
	while ((components = strtok_r(NULL, ".", &dest_temp)) != NULL)
	{
		count++;
	}
	return count;

}
/*Check if a given string contains the token, return pointer to start of that token if found*/
char* check_tag(char* string, char* token)
{
	return strstr(string,token);
}

bool extract(char* source_string, Href_url* href_url_element)
{

	char* source_string_copy;
	char* source_string_copy_dots;
	char *components, *dest_temp;
	
	source_string_copy_dots = (char*) malloc(strlen(source_string));
	source_string_copy = (char*) malloc(strlen(source_string));
	
	//jump string pointer to point to start of url
	strcpy(source_string_copy_dots, source_string);
	//tokenise with 1st quotation char
	components = strtok_r(source_string_copy_dots, "\"", &dest_temp);
	strcpy(source_string_copy, components);
	
	int dots = count_dots(components);
	
//	components = strtok_r(source_string_copy, "\"", &dest_temp);
//	printf("\nstring here is %s with dots %d\n",source_string_copy, dots);
	if(dots == 0)
	{
		/*relative path to no file*/
		/*disregard?????????
		 * ??????
		 * ?????
		 * ????
		 * ???
		 * ??
		 * ?*/
	}
	if(dots == 1)
	{
		/*it is a relative resource file*/

		//since resource, no need to VALIDATE URL------------- BEFORE ADDING??

		strncpy(href_url_element->resource_filename, components, strlen(components));
	}
	if(dots == 2)
	{
		/*it is a relative resource file
		 * component 1 and 2 are host
		 * component 3 is resource file
		 */


		//VALIDATE Hostname------------- BEFORE ADDING
		components = strtok_r(source_string_copy, "/", &dest_temp);

		
		int ret = true;//match_host(components);
		if(ret == 0)
		{
		//	printf("\nnot added this to crawler %s\n",components);
			return false;
		}
		strncpy(href_url_element->hostname, components, strlen(components));
		components = strtok_r(NULL, "", &dest_temp);
		strncpy(href_url_element->resource_filename, components, strlen(components));
		free_ptr(source_string_copy);
		free_ptr(source_string_copy_dots);
		return true;
	}
	free_ptr(source_string_copy);
	free_ptr(source_string_copy_dots);
	return false;
}
#if 0
	/*
	components = strtok_r(source_string_copy, ".", &dest_temp);
	printf("\ncomponent is %s\n", components);
	strncpy(href_url_element->hostname, components, strlen(components));
	int counter=0;
	do{
		counter++;
		printf("\ncomponent is %s\n", components);
		if(counter == 2)
			strncpy(href_url_element->resource_filename, components, strlen(components));
	}while ((components = strtok_r(NULL, "\"", &dest_temp)) != NULL);
	*/
	return NULL;
}
#endif
bool extract_url(char* source_string, Href_url* href_url_element)
{
	//printf("\nextract url for %s\n",source_string);
	char* ptr = NULL;
	char* save_ptr = NULL;
	save_ptr = (char*) malloc(512);
	if(!save_ptr)
	{
		fprintf(stderr,"\nmalloc failed\n");
		return false;
	}
	ptr = check_tag(source_string,"http://");
	if(ptr)
	{
		//strncpy(save_ptr, ptr, strlen(ptr));
		strcpy(save_ptr, ptr + strlen("http://"));//, strlen(ptr));
	}
	else
	{
		ptr = check_tag(source_string,"HTTP://");
		if(ptr)
		{
			strcpy(save_ptr, ptr+ strlen("http://"));//, strlen(ptr));
		}
		else
		{
			strncpy(save_ptr, source_string, strlen(source_string));
		}
	}
	if(extract(save_ptr, href_url_element) == true)
	{
		free_ptr(save_ptr);
		return true;
	}
	else
	{
		free_ptr(save_ptr);
		return false;
	}
	return false;
}

bool extract_href_url(char* source_string, Href_url* href_url_element)
{

	//printf("\nextract href url for %s\n",source_string);
	char* ptr = NULL;
	char* save_ptr = NULL;
	ptr = check_tag(source_string,"href =");
	if(ptr)
	{
		save_ptr = (char*) malloc(512);
		if(!save_ptr)
		{
			fprintf(stderr,"\nmalloc failed\n");
			return false;
		}
		strcpy(save_ptr, ptr + strlen("href ="));
		if(extract_url(save_ptr, href_url_element) == true)
		{
			free_ptr(save_ptr);
			return true;
		}
		else
		{
			free_ptr(save_ptr);
			return false;
		}
	}
	ptr = NULL;
	ptr = check_tag(source_string,"HREF=");
	if(ptr)
	{
		save_ptr = (char*) malloc(512);
		if(!save_ptr)
		{
			fprintf(stderr,"\nmalloc failed\n");
			return false;
		}
		strcpy(save_ptr, ptr + strlen("HREF="));
		if(extract_url(save_ptr, href_url_element) == true)
		{
			free_ptr(save_ptr);
			return true;
		}
		else
		{
			free_ptr(save_ptr);
			return false;
		}
	}
	ptr = NULL;
	ptr = check_tag(source_string,"href=");
	if(ptr)
	{	
		save_ptr = (char*) malloc(512);
		if(!save_ptr)
		{
			fprintf(stderr,"\nmalloc failed\n");
			return false;
		}
		strcpy(save_ptr, ptr + strlen("href="));
		if(extract_url(save_ptr, href_url_element) == true)
		{
			free_ptr(save_ptr);
			return true;
		}
		else
		{
			free_ptr(save_ptr);
			return false;
		}
	}
	return false;
}

int read_file(char* filename, Web_crawler* crawler_obj)
{
	FILE* file = fopen(filename, "r"); /* should check the result */
	size_t len = 512;
	char full_line[len];
	char full_line_copy[len];
	int index = crawler_obj->href_url_count;//add data to last valid index

	if(!file)
	{
		printf("\nfailed to open %s\n",filename);
		return 0;
	}
	while(fgets(full_line, 512, file)) {
		/*remove newline character*/
		full_line[strlen(full_line) - 1]='\0';
		if(strlen(full_line) > 0)
		{
			if((check_tag(full_line,"<a") != NULL) || (check_tag(full_line,"<A")) != NULL)
			{
				strcpy(full_line_copy, full_line);
				if(extract_href_url(full_line_copy, &crawler_obj->href_url[index]) 
						== true)
				{	
					index++;
				}
				else
				{
					fprintf(stderr,"\n________********________*****\nfalse line %s\nnot adding\n----****____****\n",full_line_copy);
					memset(&(crawler_obj->href_url[index]).resource_filename,'\0', 512);
					memset(&(crawler_obj->href_url[index]).hostname,'\0', 32);
					crawler_obj->href_url[index].to_visit = false;
				}
			}
		}
		memset(full_line,'\0',512);
	}
	crawler_obj->href_url_count = index;
	return crawler_obj->href_url_count;
}

int main(int argc, char **argv)
{
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

	
	href_count = read_file("local_html_file.html", &crawler);
	//now file is useles, valid content copied to crawler_obj, can delete
	
	printf("\ninitial values read : %d\n",href_count);
//	int socket = initialise_socket();
	for(i=0;i<href_count;i++)
	{
		printf("\ncrawler.href_url[%d].resource_filename : %s",i,crawler.href_url[i].resource_filename);
		printf("\ncrawler.href_url[%d].hostname : %s\n",i,crawler.href_url[i].hostname);
		
//		send_receive_socket_data(socket, crawler.href_url[i].resource_filename, i);

		href_count = read_file("local_html_file_1.html", &crawler);
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
