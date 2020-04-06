


#include "file_parser.h"
#define MAX_RESOURCE_LEN 512

extern char* original_host;

int count_dots(char* string)
{
	char *components, *dest_temp;
	int count=0;
	components = strtok_r(string, ".", &dest_temp);
	if(components != NULL) 
	{
		count++;
	}
	else
	{
		return count;
	}
	while ((components = strtok_r(NULL, ".", &dest_temp)) != NULL)
	{
		count++;
	}
//	do {
//		count++;
	//	printf("\ncomponent[%d] is %s\n",count,components);
//	} while ((components = strtok_r(NULL, ".", &dest_temp)) != NULL);
	return count;

}
int fill_constituents(char** constituent, char* url)
{
	char *components, *dest_temp;
	fprintf(stderr, "\nurl is %s\n",url);
	components = strtok_r(url, ".", &dest_temp);
	int count=0;
	do {
		constituent[count] = (char*) calloc(strlen(components),sizeof(char));
		strcpy(constituent[count], components);
		count++;
		if(count > 3)
		{
			return 1;
		}
	} while ((components = strtok_r(NULL, ".", &dest_temp)) != NULL);
	return 0;

}
char* get_resource_name(char** constituent, int count)
{
	//int new_len = strlen(constituent[count-2]) + strlen(constituent[count-1]);
	int new_len = MAX_RESOURCE_LEN;
	char* resource_name;
	char *dst, *dest_temp;
	constituent[count-2] = realloc(constituent[count-2], new_len);
	if(constituent[count-2] == NULL)
	{
		printf("\nrealloc failed\n");
	}
	strcat(constituent[count-2],".");
	strcat(constituent[count-2], constituent[count-1]);

	resource_name = (char*) calloc(strlen(constituent[count-2]), sizeof(char));
	strcpy(resource_name, constituent[count-2]);

	dst = strtok_r(resource_name, "/", &dest_temp);

	new_len = strlen(dst);
	memset(constituent[count-2],'\0',strlen(constituent[count-2]));
	strncpy(constituent[count-2], dst, strlen(dst));
	return dest_temp;
}


int match_host(Href_url* href_url)
{
	char** constituents_href_tag;
	char** constituents_original_host;

	char* href_copy = (char*)malloc(strlen(href_url->hostname));
	char* hostname_copy = (char*)malloc(strlen(original_host));
	
	strcpy(href_copy, href_url->hostname);
	strcpy(hostname_copy,original_host);

	fprintf(stderr,"\ncounting components for %s\n", href_copy);
	int components_host = count_dots(hostname_copy);
	int components_href = count_dots(href_copy);
	
	strcpy(href_copy, href_url->hostname);
	strcpy(hostname_copy,original_host);
	
	constituents_href_tag = (char**) calloc(components_href, sizeof(char*));
	constituents_original_host = (char**) calloc(components_host, sizeof(char*));
	if(components_host != 0)
	{
		fill_constituents(constituents_original_host, hostname_copy);
	}
/*
	else
	{
		constituents_original_host[components_host] = (char*) calloc(strlen(hostname_copy),sizeof(char));
		strcpy(constituents_original_host[components_host], hostname_copy);
	}
*/	
	if(components_href != 0)
	{
		fill_constituents(constituents_href_tag, href_copy);
	}
/*
	else
	{
		constituents_href_tag[components_href] = (char*) calloc(strlen(href_copy),sizeof(char));
		strcpy(constituents_href_tag[components_href], href_copy);
	}
*/	
	int i=0;
	for(i=0; i<components_host; i++)
	{	
		fprintf(stderr,"\noriginal host [%d] is %s\n",i,constituents_original_host[i]);
	}
	for(i=0; i<components_href; i++)
	{	
		fprintf(stderr,"\n tag host [%d] is %s\n",i,constituents_href_tag[i]);
	}
	int count = components_host > components_href? components_href: components_host;
	//int count = components_host == components_href? components_host: components_href;

	int x;
	fprintf(stderr,"\ncomponents_host %d, components_href %d, count = %d\n",components_host, components_href, count);


	for(i=count-1; i> 0; i--)
	{
		if(strcmp(constituents_original_host[i], constituents_href_tag[i]) == 0)
			x++;
	}
	if(x == count-1)
	{
		//printf("\nCrawl this url\n");
		return 1;
	}
	else
	{
//		printf("\nDont Crawl this url\n");
		return 0;
	}
	return 0;
}
int validate_url(char* url, Href_url *href_url)
{
	char* url_copy =(char*) calloc(strlen(url), sizeof(char));
	strncpy(url_copy, url, strlen(url));
	
	/*
	 * this returns number of dots, do components actually dots + 1
	 */
	int components = count_dots(url);
	char** constituents;

	constituents = (char**) calloc(components, sizeof(char*));

	fill_constituents(constituents, url_copy);
	
	if(components == 0)
	{
		/*This means that the url is a relative path*/
//		href_url->hostname = (char*) calloc(strlen(original_host), sizeof(char));
//		strcpy(href_url->hostname, original_host);
//		or
		href_url->hostname = NULL;
		href_url->resource_filename = (char*)calloc(strlen(constituents[components-0]), sizeof(char));
		strcpy(href_url->resource_filename, constituents[components-0]);
	}
	if(components == 1)
	{
		components++;
		//this means dots were 1, so dot separated components are 2	
		//comp[0] is host
		//comp[1] is resource
		href_url->hostname = (char*) calloc(strlen(constituents[components-2]), sizeof(char));
		strcpy(href_url->hostname,constituents[components-2]);
		if((strstr(href_url->hostname,"http://") !=NULL) || (strstr(href_url->hostname,"HTTP://") !=NULL))
		{
			strcpy(href_url->hostname, href_url->hostname+7);
		}
		href_url->resource_filename = (char*)calloc(strlen(constituents[components-1]), sizeof(char));
		strcpy(href_url->resource_filename, constituents[components-1]);
	}
	if(components == 2)
	{
		components++;
		//this means dots were 2, so dot separated components are 3	
		//comp[0]/comp[1].. is host
		//..comp[1]/comp[2] is resource
		char* host_name = malloc(strlen(constituents[components-2]));
		strcpy(host_name,constituents[components-2]);
		char *dst, *dest_temp;
		dst = strtok_r(host_name, "/", &dest_temp);
		strcpy(href_url->hostname, dst);
		free(host_name);
		host_name = NULL;
		
		if((strstr(href_url->hostname,"http://") !=NULL) || (strstr(href_url->hostname,"HTTP://") !=NULL))
		{
			strcpy(href_url->hostname, href_url->hostname+7);
		}
		int len = MAX_RESOURCE_LEN;
		//int len = strlen(constituents[components-2]) + strlen(constituents[components-1]);
		href_url->resource_filename = (char*) calloc(len , sizeof(char));
		strcpy(href_url->resource_filename,get_resource_name(constituents, components));
		

	}
	if(components == 3)
	{
		href_url->resource_filename = (char*) calloc(strlen(constituents[components-2] + strlen(constituents[components-1])), sizeof(char));
		strcpy(href_url->resource_filename,get_resource_name(constituents, components));
		href_url->hostname = (char*) calloc(strlen(constituents[0] + strlen(constituents[1])) + 1, sizeof(char));
		strcat(href_url->hostname,constituents[components-3]);
		strcat(href_url->hostname,".");
		strcat(href_url->hostname,constituents[components-2]);
		if((strstr(href_url->hostname,"http://") !=NULL) || (strstr(href_url->hostname,"HTTP://") !=NULL))
		{
			strcpy(href_url->hostname, href_url->hostname+7);
		}
//		free(hostname);
		
	}

	if(components == 4)
	{
		href_url->resource_filename = (char*) calloc(strlen(constituents[components-3] + strlen(constituents[components-2])), sizeof(char));
		href_url->resource_filename = get_resource_name(constituents, components);
		href_url->hostname = (char*) calloc(strlen(constituents[components-3] + strlen(constituents[components-2])) + 1, sizeof(char));
		strcat(href_url->hostname,constituents[components-3]);
		strcat(href_url->hostname,".");
		strcat(href_url->hostname,constituents[components-2]);
				
		if((strstr(href_url->hostname,"http://") !=NULL) || (strstr(href_url->hostname,"HTTP://") !=NULL))
		{
			strcpy(href_url->hostname, href_url->hostname+7);
		}
//		free(hostname);
	}
	/*
	for(i=0; i<components; i++)
	{
		if(constituents[i] && (strlen(constituents[i])!=0))
		{
			free(constituents[i]);
		}
	}
	if(constituents)
		free(constituents);
		*/
	int ret;
	ret = match_host(href_url);

	free(url_copy);
	return ret;
}

int extract_validate_href(char* string, Href_url* href_url)
{
	char *dest_string, *dest_temp;
	char* href_start = strstr(string,"<a href=");
	if(href_start == NULL)
		href_start = strstr(string,"<A HREF=");
	fprintf(stderr,"\n---------------------------------\nvalidating url %s\n\n",href_start);
	dest_string = strtok_r(href_start, "\"", &dest_temp);
	dest_string = strtok_r(NULL, "\"", &dest_temp);
	fprintf(stderr,"\n---------------------------------\n url %s\n\n",dest_string);
	return validate_url(dest_string, href_url);
	
}


void* parse_html_file(char* filename, Web_crawler *crawler)
{
	FILE* file = fopen(filename, "r"); /* should check the result */
	size_t len = 512;
	char* full_line;
	
	Href_url href_url;
       	full_line = (char*) calloc(len, sizeof(char));
	if(!full_line)
	{
		printf("\nfailed to allocate memory\n");
		return NULL;
	}

	if(!file)
	{
		printf("\nfailed to open %s\n",filename);
		return NULL;
	}
	while(fgets(full_line, 512, file)) {
		/*remove newline character*/
		full_line[strlen(full_line)-1]='\0';
		if(full_line)
		{
			/*
			 * The basic extension specifies all anchor tags. 
			 * It's possible that some 
			 * variations will end up in test cases.
			   Here are some of them:
			  <A HREF="/">link</a>
			  <a href='/'>link</a>
			  <a target="_blank" href='/' rel="noreferrer">link</a>
			  <a    href  =   '/'>link</a>
			 */

			if(strstr(full_line, "<a href") != NULL)
			{
				fprintf(stderr,"\nchecking url : %s\n",full_line);
				href_url.to_visit = 0;
				href_url.to_visit = extract_validate_href(full_line, &href_url);
				if(href_url.to_visit == 1)
				{
					//add this to the queue
					crawler->visit[crawler->visited_count] = 0;
					crawler->visited_url[crawler->visited_count] = malloc(strlen(href_url.hostname) + strlen(href_url.resource_filename)+10);
					sprintf(crawler->visited_url[crawler->visited_count],"http://%s/%s\n",href_url.hostname, href_url.resource_filename);
					fprintf(stderr,"\n****************************************************************************\n");
					fprintf(stderr,"\nvalue added to array is :%s\n",crawler->visited_url[crawler->visited_count]);
					fprintf(stderr,"\n****************************************************************************\n");
					crawler->visited_count++;

				//	}	
				}
				else
				{
					fprintf(stderr,"\nvalue not added to array :%s/%s\n",href_url.hostname, href_url.resource_filename);
				}
			}
		}
	}
	free(full_line);
	fclose(file);
	return NULL;
}

