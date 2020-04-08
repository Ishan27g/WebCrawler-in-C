#include "socket_operations.h"


extern char* original_host;

int initialise_socket()
{
	int client_socket;
	struct hostent * localhost_server;
        struct sockaddr_in server_socket;
        memset(&server_socket, 0, sizeof(server_socket));
	
        /* Construct the server_socket structure */
	fprintf( stderr,"\noriginal_host is %s\n",original_host);
	localhost_server = gethostbyname(original_host);
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
            perror("ERROR opening socket\n\n");
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
void* send_receive_socket_data(int client_socket, char* resource, int count)
{
	char request_str[512];
        int len;
        FILE *received_file;
	char buffer[BUFSIZ];
	char* html_content = NULL;
	int n;
	Http_header http_head;
	if(resource == NULL)
	{
		len = HTTP_REQ_STR_LEN + strlen("GET /") + 1;
		memset(request_str,'\0',512);
		sprintf(request_str,"GET / %s",HTTP_REQ_STR);
		request_str[strlen(request_str)] = '\0';
//		fprintf( stderr,"\nSending request :\n[%s]\n",request_str);
	}
	else
	{
		len = HTTP_REQ_STR_LEN + strlen("GET /") + strlen(resource) + 1;
		memset(request_str,'\0',512);
		sprintf(request_str,"GET /%s %s",resource, HTTP_REQ_STR);
		request_str[strlen(request_str)] = '\0';
//		fprintf( stderr,"\nSending request :\n[%s]\n",request_str);
	}
	n = write(client_socket, request_str, strlen(request_str));
	if( n<0)
	{
		fprintf( stderr,"\nError in sending request\n");
	}
	else
	{
		fprintf( stderr,"\nSent request\n%s", request_str);
	}
	char local_file[32];
	sprintf(local_file, "%s_%d", HTML_FILE_LOCAL, count);
	received_file = fopen(local_file, "w");
        if (received_file == NULL)
        {
		fprintf( stderr,"Failed to open local file");
		return NULL;
        }
        /* Receiving file size */
	len = recv(client_socket, buffer, 512, 0);
	fprintf( stderr,"\nResponse message of length: %d\n\n",len);
	
	//http_head = malloc(sizeof(Http_header) + 512);
	//
	memset(http_head.http_version,'\0',sizeof(http_head.http_version));
	memset(http_head.http_rsp_code,'\0',sizeof(http_head.http_rsp_code));
	memset(http_head.http_server,'\0',sizeof(http_head.http_server));
	memset(http_head.http_content_type,'\0',sizeof(http_head.http_content_type));
	html_content = (char*)malloc(512);
	memset(html_content,'\0',512);

	get_http_header(buffer, &http_head, html_content);

//	fprintf(stderr,"\n*******____________HTTP RESPONSE HEADER__________*******_______________\n");
	if(http_head.http_version)
		fprintf(stderr,"\nversion is %s",http_head.http_version);
	if(http_head.http_rsp_code)
		fprintf(stderr,"\ncode is %s",http_head.http_rsp_code);
	if(http_head.http_server)
		fprintf(stderr,"\nServer is %s",http_head.http_server);
	if(http_head.http_content_type)
		fprintf(stderr,"\ncontent type is [%s]",http_head.http_content_type);
	
//	fprintf(stderr,"\n*******____________*********___________*******_______________\n");
//	fprintf(stderr,"\ncontent length is %d\n",http_head.http_content_length);
//	fprintf(stderr,"\ncontent is %s\n",html_content);
	/*
	 * validate content type = txt/html in html_content->http_content_type
	 * */
/*	if(strcmp(http_head.http_content_type, " text/html") != 0)
	{
		fprintf( stderr,"\nContent type is not text/html\n");
		return NULL;
	}
*/
	fwrite(html_content, sizeof(char), strlen(html_content), received_file);
	while(len > 0){
		len = recv(client_socket, buffer, 512, 0);
//		fprintf(stderr,"\n %s\n ",buffer);
                fwrite(buffer, sizeof(char), len, received_file);
	}
	free(html_content);
        fclose(received_file);
	return NULL;
}
