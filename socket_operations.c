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
void* send_receive_socket_data(int client_socket, char* resource)
{
	char *request_str = NULL;
        int len;
        FILE *received_file;
	char buffer[BUFSIZ];
	char* html_content = NULL;
	int n;
	Http_header http_head;
	if(resource == NULL)
	{
		len = HTTP_REQ_STR_LEN + strlen("GET /") + 1;
		request_str = (char*) malloc(len);
		memset(request_str,'\0',len);
		sprintf(request_str,"GET / %s",HTTP_REQ_STR);
		request_str[strlen(request_str)] = '\0';
		fprintf( stderr,"\nSending request :\n[%s]\n",request_str);
	}
	else
	{
		len = HTTP_REQ_STR_LEN + strlen("GET /") + strlen(resource) + 1;
		request_str = (char*) malloc(len);
		memset(request_str,'\0',len);
		sprintf(request_str,"GET /%s %s",resource, HTTP_REQ_STR);
		request_str[strlen(request_str)] = '\0';
		fprintf( stderr,"\nSending request :\n[%s]\n",request_str);
	}
	n = write(client_socket, request_str, strlen(request_str));
	if( n<0)
	{
		fprintf( stderr,"\nError in sending request\n");
	}
	else
	{
		fprintf( stderr,"\nSent request\n\n %s\n", request_str);
	}
	free(request_str);
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
	
	if(http_head.http_version)
		fprintf(stderr,"\nversion is %s\n",http_head.http_version);
	if(http_head.http_rsp_code)
		fprintf(stderr,"\ncode is %s\n",http_head.http_rsp_code);
	if(http_head.http_server)
		fprintf(stderr,"\nServer is %s\n",http_head.http_server);
	if(http_head.http_content_type)
		fprintf(stderr,"\ncontent type is %s\n",http_head.http_content_type);
	
	fprintf(stderr,"\ncontent length is %d\n",http_head.http_content_length);
	/*
	 * validate content type = txt/html in html_content->http_content_type
	 * */
	if(strcmp(http_head.http_content_type, " text/html") != 0)
	{
		fprintf( stderr,"\nContent type is not text/html\n");
		return NULL;
	}
	received_file = fopen(HTML_FILE_LOCAL, "w");
        if (received_file == NULL)
        {
		fprintf( stderr,"Failed to open local file");
		return NULL;
        }

	fwrite(html_content, sizeof(char), strlen(html_content), received_file);
	while(len > 0){
		len = recv(client_socket, buffer, 512, 0);
                fwrite(buffer, sizeof(char), len, received_file);
	}
        fclose(received_file);
	return NULL;
}
