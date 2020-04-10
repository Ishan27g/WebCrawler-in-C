
#include "html_file_parser.h"

extern char* original_host;
extern Web_crawler crawler;

void* free_ptr(void* ptr)
{
	if(ptr)
		free(ptr);
	ptr = NULL;
	return NULL;
}

int lookup_duplicate_page(char* pagename)
{
	int i=0;
	for(i=0; i < crawler.href_url_count; i++)
	{
		if(strcmp(pagename, crawler.href_url[i].resource_filename) == 0)
		{
			return 0;
		}
	}
	return 1;
}


int match_host(char* href_host)
{
	if(href_host == NULL)
	{
		fprintf(stderr,"\nhost is null, not present in line, dont add the crawler_obj->href_element");
		return 0;
	}
	char* href_copy = (char*)malloc(strlen(href_host));
	char* hostname_copy = (char*)malloc(strlen(original_host));

	strcpy(href_copy, href_host);
	strcpy(hostname_copy, original_host);
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

	strcpy(source_string_copy, source_string);
	strcpy(source_string_copy_dots, source_string);

	int ret;	
	int dots;

	components = strstr(source_string_copy, "\"//");
	if(components)
	{
		strcpy(source_string_copy_dots, components + 3);
	}
	components = strtok_r(source_string_copy_dots, "\"", &dest_temp);
	strcpy(source_string_copy, components);
	//jump string pointer to point to start of url
	//tokenise with 1st quotation char
	dots = count_dots(components);
	if(dots == 1)
	{
		/*it is a relative resource file*/

		strncpy(href_url_element->hostname, original_host, strlen(original_host));

		//since resource, no need to VALIDATE URL------------- BEFORE ADDING??
		if(strstr(components, "/") == NULL)
		{
			sprintf(href_url_element->resource_filename,"%s.html",components);
		}
		else
		{
			strncpy(href_url_element->resource_filename, components, strlen(components));
		}
		ret = lookup_duplicate_page(href_url_element->resource_filename);
		if(ret ==1)
		{
			href_url_element->relative = 1;
			return true;
		}
		else
			return false;
	}
	if(dots == 2)
	{
		/*it is a relative resource file
		 * component 1 and 2 are host
		 * component 3 is resource file
		 */

		components = strtok_r(source_string_copy, "/", &dest_temp);

		ret = match_host(components);
		if(ret == 0)
		{
			return false;
		}
		strncpy(href_url_element->hostname, components, strlen(components));
		components = strtok_r(NULL, "", &dest_temp);
		strcpy(href_url_element->resource_filename, components);
		ret = lookup_duplicate_page(href_url_element->resource_filename);
		if(ret ==1)
			return true;
		else
			return false;
	}
	return false;
}

bool extract_url(char* source_string, Href_url* href_url_element)
{
	char* ptr = NULL;
	char* save_ptr = NULL;
	save_ptr = (char*) malloc(strlen(source_string));
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
			if(strncmp(source_string+1, "//",2) == 0)
			{
				strcpy(save_ptr, source_string + 3);
			}
			else
			{
				strncpy(save_ptr, source_string, strlen(source_string));
			}
		}
	}
	if(extract(save_ptr, href_url_element) == true)
	{
		free_ptr(save_ptr);
		return true;
	}
	else
	{
		return false;
	}
	return false;
}

bool extract_href_url(char* source_string, Href_url* href_url_element)
{

	char* ptr = NULL;
	char* save_ptr = NULL;
	ptr = check_tag(source_string,"href=");
	if(ptr)
	{	
		save_ptr = (char*) malloc(strlen(source_string));
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
	ptr = NULL;
	ptr = check_tag(source_string,"href =");
	if(ptr)
	{
		save_ptr = (char*) malloc(strlen(source_string));
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
	ptr = check_tag(source_string,"href = ");
	if(ptr)
	{
		save_ptr = (char*) malloc(strlen(source_string));
		if(!save_ptr)
		{
			fprintf(stderr,"\nmalloc failed\n");
			return false;
		}
		strcpy(save_ptr, ptr + strlen("href = "));
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
	ptr = check_tag(source_string,"href= ");
	if(ptr)
	{	
		save_ptr = (char*) malloc(strlen(source_string));
		if(!save_ptr)
		{
			fprintf(stderr,"\nmalloc failed\n");
			return false;
		}
		strcpy(save_ptr, ptr + strlen("href= "));
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
		save_ptr = (char*) malloc(strlen(source_string));
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
	ptr = check_tag(source_string,"HREF =");
	if(ptr)
	{
		save_ptr = (char*) malloc(strlen(source_string));
		if(!save_ptr)
		{
			fprintf(stderr,"\nmalloc failed\n");
			return false;
		}
		strcpy(save_ptr, ptr + strlen("HREF ="));
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
	ptr = check_tag(source_string,"HREF = ");
	if(ptr)
	{
		save_ptr = (char*) malloc(strlen(source_string));
		if(!save_ptr)
		{
			fprintf(stderr,"\nmalloc failed\n");
			return false;
		}
		strcpy(save_ptr, ptr + strlen("HREF = "));
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
	ptr = check_tag(source_string,"HREF= ");
	if(ptr)
	{
		save_ptr = (char*) malloc(strlen(source_string));
		if(!save_ptr)
		{
			fprintf(stderr,"\nmalloc failed\n");
			return false;
		}
		strcpy(save_ptr, ptr + strlen("HREF= "));
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

int read_file(char* filename)
{
	FILE* file = fopen(HTML_FILE_LOCAL, "r"); /* should check the result */
	size_t len = 1024;
	char full_line[len];
	char full_line_copy[len];
	int index = crawler.href_url_count;//add data to last valid index

	if(!file)
	{
		fprintf(stderr,"\nfailed to open %s\n",HTML_FILE_LOCAL);
		return 0;
	}
	while(fgets(full_line, 1024, file)) {
			if(strstr(full_line, ".html") != NULL)
			{
				if((check_tag(full_line,"<a") != NULL) || (check_tag(full_line,"<A")) != NULL)
				{
					if(strlen(full_line) > (strlen("<a href=\"//") + strlen(".html") + 1))
					{

						strcpy(full_line_copy, full_line);
						if(extract_href_url(full_line_copy, &(crawler.href_url[index])) 
								== true)
						{	
							index++;
							crawler.href_url_count++;
						}
						else
						{
							memset(crawler.href_url[index].resource_filename,'\0', 1024);
							memset(crawler.href_url[index].hostname,'\0', 32);
						}
					}
				}
			}
		memset(full_line,'\0',1024);
	}
	fclose(file);
	crawler.href_url_count = index;
	return index;
}

