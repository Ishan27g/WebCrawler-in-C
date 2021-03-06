#include <sys/stat.h>
#include "socket_operations.h"


extern char* original_host;
extern Http_extension http_extension;

int validate_rsp_code(char* http_rsp_code)
{
	if(strstr(http_rsp_code, "200") != NULL)
	{
		return HTTP_RSP_200_SUCCESS;
	}	
	if(strstr(http_rsp_code, "404") != NULL)
	{
		return HTTP_RSP_404_NOT_FOUND;
	}	
	if(strstr(http_rsp_code, "410") != NULL)
	{
		return HTTP_RSP_410_PERM_GONE;
	}	
	if(strstr(http_rsp_code, "414") != NULL)
	{
		return HTTP_RSP_414_REQ_URI_LONG;
		//can be a redirected URI prefix that points to a suffix of itself
	}	
	if(strstr(http_rsp_code, "503") != NULL)
	{
		return HTTP_RSP_503_SERVICE_UNAVAILABLE;
		//read Retry-After to retry after that much time
	}	
	if(strstr(http_rsp_code, "504") != NULL)
	{
		return HTTP_RSP_504_GW_TIMEOUT;
	}
	if(strstr(http_rsp_code, "301") != NULL)
	{
		return HTTP_RSP_301_MOVED_PERM;
	}
	if(strstr(http_rsp_code, "401") != NULL)
	{
		return HTTP_RSP_401_NOT_AUTH;
	}
	else
		return 100;	
}
int initialise_socket(char* crawling_host)
{
	int client_socket;
	struct hostent * localhost_server;
        struct sockaddr_in server_socket;
        memset(&server_socket, 0, sizeof(server_socket));
	
        /* Construct the server_socket structure */
	if(strlen(crawling_host) == 0)
	{
		localhost_server = gethostbyname(original_host);
	}
	else
	{
		localhost_server = gethostbyname(crawling_host);
	}
	if(!localhost_server)
	{
		fprintf( stderr,"\nERROR, no such host\n");
		return 0;
	}	
        
	server_socket.sin_family = AF_INET;
	bcopy(localhost_server->h_addr_list[0], (char *)&server_socket.sin_addr.s_addr, localhost_server->h_length);
        server_socket.sin_port = htons(PORT_NUMBER);
        
	/* Create client TCP socket */

        client_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (client_socket < 0)
        {
            fprintf( stderr,"\nERROR opening socket\n\n");
	    return 0;
        }

        /* Connect to the server */
        if (connect(client_socket, (struct sockaddr *)&server_socket, sizeof(struct sockaddr)) == -1)
        {
                fprintf( stderr,"\nError on connect %s\n\n", strerror(errno));

                exit(EXIT_FAILURE);
        }
	return client_socket;
}
int send_receive_socket_data(int client_socket, char* resource, int flag, int relative)
{
	char deletethis[1024];


	char request_str[1024];
	int len;
        FILE *received_file;
	char buffer[1024];
	char html_content[1024];
	int n;
	Http_header http_head;
	if(resource == NULL)
	{
		memset(request_str,'\0',1024);
		sprintf(request_str,"GET / %s",HTTP_REQ_STR);
	}
	else
	{
		if(flag == HTTP_RSP_401_NOT_AUTH)
		{
			memset(request_str,'\0',1024);
			if(relative == 1)
			{	
				sprintf(request_str,"GET %s %s",resource, HTTP_AUTH_STR);
			}
			else
			{
				sprintf(request_str,"GET /%s %s",resource, HTTP_AUTH_STR);
			}
		}
		else
		{
			memset(request_str,'\0',1024);
			if(relative == 1)
			{	
				sprintf(request_str,"GET %s %s",resource, HTTP_REQ_STR);
			}
			else
			{
				sprintf(request_str,"GET /%s %s",resource, HTTP_REQ_STR);
			}
		}
	}
	n = write(client_socket, request_str, strlen(request_str));
	if( n<0)
	{
//		fprintf( stderr,"\nError in sending request\n");
	}
	else
	{
//		fprintf( stderr,"\nSent request\n%s", request_str);
	}
        /* Receiving file size */
	len = recv(client_socket, buffer, 1000, 0);
	memset(http_head.http_version,'\0',sizeof(http_head.http_version));
	memset(http_head.http_rsp_code,'\0',sizeof(http_head.http_rsp_code));
	memset(http_head.http_server,'\0',sizeof(http_head.http_server));
	memset(http_head.http_content_type,'\0',sizeof(http_head.http_content_type));
	http_head.http_content_length = 0;
	
	memset(html_content,'\0',1024);

	memset(deletethis,'\0',1024);
	strncpy(deletethis, buffer, len);

	get_http_header(buffer, &http_head, &html_content[0]);
	
	int data_remaining = len > http_head.http_content_length? 0: http_head.http_content_length - len;
	int http_rsp_code = validate_rsp_code(http_head.http_rsp_code);
	switch (http_rsp_code)
	{	
		case HTTP_RSP_503_SERVICE_UNAVAILABLE:
		case HTTP_RSP_504_GW_TIMEOUT:
			return HTTP_RSP_504_GW_TIMEOUT;//retry after
		case HTTP_RSP_200_SUCCESS:
			break;
		case HTTP_RSP_301_MOVED_PERM:
			strncpy(http_extension.http_location, http_head.http_location, strlen(http_head.http_location));
			return HTTP_RSP_301_MOVED_PERM;
		case HTTP_RSP_401_NOT_AUTH:
			return HTTP_RSP_401_NOT_AUTH;
		case HTTP_RSP_414_REQ_URI_LONG:
		case HTTP_RSP_410_PERM_GONE:
		case HTTP_RSP_404_NOT_FOUND:
		default:
			return 0;
	}
	if(strstr(http_head.http_content_type, MIME_TYPE_TEXT_HTML) == NULL)
	{
		return 0;
	}
	received_file = fopen(HTML_FILE_LOCAL, "w");
        if (received_file == NULL)
        {
		fprintf( stderr,"Failed to open local file");
		return 0;
        }
	fwrite(html_content, sizeof(char), len, received_file);
	if(data_remaining > 0)
	{
		memset(buffer,'\0',1000);
		len = recv(client_socket, buffer, 1000, 0);
		if(len > 0)
		{
			len = fwrite(buffer, sizeof(char), len, received_file);
			data_remaining = data_remaining - len;
		}
	}
        fclose(received_file);
	return 1;
}

