
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
	fprintf(stderr,"\nwhile looking up,crawler.href_url_count is %d\n",crawler.href_url_count);
	for(i=0; i < crawler.href_url_count; i++)
	{
		fprintf(stderr,"\ncomparing [%s] with [%s]\n",pagename, crawler.href_url[i].resource_filename);
		if(strcmp(pagename, crawler.href_url[i].resource_filename) == 0)
		{
			fprintf(stderr,"\n^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
			fprintf(stderr,"\nNot crawling duplicate resource %s, already visited at %s . %s\n",pagename,
				       crawler.href_url[i].hostname, crawler.href_url[i].resource_filename);
			fprintf(stderr,"\n^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
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
//	fprintf(stderr,"\ncomparing hosts %s and %s\n",ho,hr);
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
int check_if_only_filename(char* source_string)
{
	char *components = malloc(strlen(source_string));
	strcpy(components, source_string);
	if(strstr(components,"html") == NULL)
	{
		return 0;
	}
	int dots = count_dots(components);
	if(dots == 1)
	{
		fprintf(stderr,"\nresource is a relative file\n");
		return 1;
	}
	return 0;
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

	fprintf(stderr,"\nto extract %s\n",source_string);

	int ret;	
	int dots;


	//	if(check_if_only_filename(source_string) != 1)
	//	{
	components = strstr(source_string_copy, "\"//");
	if(components)
	{
		strcpy(source_string_copy_dots, components + 3);
	}
	components = strtok_r(source_string_copy_dots, "\"", &dest_temp);
	strcpy(source_string_copy, components);
	//fprintf(stderr,"\ncopied %s\n",source_string_copy_dots);
	//jump string pointer to point to start of url
	//tokenise with 1st quotation char
	dots = count_dots(components);
	//	}
	//	else
	//	{
	//		dots = count_dots(source_string);
	//	}
	//	components = strtok_r(source_string_copy, "\"", &dest_temp);
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
			return true;
		else
			return false;
	}
	if(dots == 2)
	{
		/*it is a relative resource file
		 * component 1 and 2 are host
		 * component 3 is resource file
		 */


		//VALIDATE Hostname------------- BEFORE ADDING
		components = strtok_r(source_string_copy, "/", &dest_temp);

//#ifdef VM_DEBUG_ON
//		ret = 1;
//#else	
		ret = match_host(components);
//#endif
		if(ret == 0)
		{
			fprintf(stderr,"\n^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
			fprintf(stderr,"\nNot crawling url(host_mismatch) :  %s\n",components);
			fprintf(stderr,"\n^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
			return false;
		}
		strncpy(href_url_element->hostname, components, strlen(components));
		components = strtok_r(NULL, "", &dest_temp);
		strcpy(href_url_element->resource_filename, components);
		//strncpy(href_url_element->resource_filename, components, strlen(components));
//		free_ptr(source_string_copy);
//		free_ptr(source_string_copy_dots);
		ret = lookup_duplicate_page(href_url_element->resource_filename);
		if(ret ==1)
			return true;
		else
			return false;
	}
//	free_ptr(source_string_copy);
//	free_ptr(source_string_copy_dots);
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
		free_ptr(save_ptr);
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
// 	Web_crawler* crawler_obj = &crawler;
	int index = crawler.href_url_count;//add data to last valid index

	fprintf(stderr,"\nbefore reading file, crawler.href_url_count is %d\n",crawler.href_url_count);
	if(!file)
	{
		fprintf(stderr,"\nfailed to open %s\n",HTML_FILE_LOCAL);
		return 0;
	}
	while(fgets(full_line, 1024, file)) {
		/*remove newline character*/
		//full_line[strlen(full_line) - 1]='\0';
//		if(strlen(full_line) > 0)
//		{
//			fprintf(stderr,"\nline is [%s]",full_line);
			if((check_tag(full_line,"<a") != NULL) || (check_tag(full_line,"<A")) != NULL)
			{
				if(strlen(full_line) > (strlen("<a href=\"//") + strlen(".html") + 1))
				{

					strcpy(full_line_copy, full_line);
					fprintf(stderr,"\n^^^^$$$$^^^^extracting [%s] at index %d\n",full_line_copy,index);
					if(extract_href_url(full_line_copy, &(crawler.href_url[index])) 
							== true)
					{	
						fprintf(stderr,"\nadded\ncrawler_obj.href_url[%d].resource_filename [%s]\n",index, crawler.href_url[index].resource_filename);
						crawler.href_url[index].visited = true;
						index++;
						crawler.href_url_count++;
					}
					else
					{
						fprintf(stderr,"\nremoving\ncrawler_obj.href_url[%d].resource_filename [%s]\n",index, crawler.href_url[index].resource_filename);
						memset(crawler.href_url[index].resource_filename,'\0', 512);
						memset(crawler.href_url[index].hostname,'\0', 32);
						crawler.href_url[index].visited = false;
					}
				}
			}
	//	}
		memset(full_line,'\0',1024);
	}
	fclose(file);
	crawler.href_url_count = index;
	return index;
}
