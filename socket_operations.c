#include "socket_operations.h"


extern char* original_host;

int initialise_socket()
{
	int client_socket;
	struct hostent * localhost_server;
        struct sockaddr_in server_socket;
        memset(&server_socket, 0, sizeof(server_socket));
	
        /* Construct the server_socket structure */
	localhost_server = gethostbyname(original_host);
	if(!localhost_server)
	{
		fprintf(stderr, "ERROR, no such host\n");
		exit(0);
	}	
        
	server_socket.sin_family = AF_INET;
	bcopy(localhost_server->h_addr_list[0], (char *)&server_socket.sin_addr.s_addr, localhost_server->h_length);
        server_socket.sin_port = htons(PORT_NUMBER);
        
	/* Create client TCP socket */

        client_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (client_socket < 0)
        {
            perror("ERROR opening socket");
            exit(0);
        }
	printf("\nclient_socket is %d\n",client_socket);

        /* Connect to the server */
        if (connect(client_socket, (struct sockaddr *)&server_socket, sizeof(struct sockaddr)) == -1)
        {
                fprintf(stderr, "Error on connect %s\n", strerror(errno));

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
	Http_header *http_head;
	http_head = malloc(sizeof(Http_header));
	
	html_content = (char*)malloc(512);
	memset(html_content,'\0',512);
	if(resource == NULL)
	{
		request_str = (char*) malloc(HTTP_REQ_STR_LEN);
		memset(request_str,'\0',HTTP_REQ_STR_LEN);
		//strcpy(request_str,HTTP_REQ_STR);
		sprintf(request_str,"GET / %s",HTTP_REQ_STR);
		request_str[strlen(request_str)] = '\0';
		printf("\nSending request :\n[%s]\n",request_str);
	}
	else
	{
		resource[strlen(resource)-1]='\0';
		request_str = (char*) malloc(HTTP_REQ_STR_LEN + strlen(resource));
		memset(request_str,'\0',HTTP_REQ_STR_LEN + strlen(resource) + 1);
		sprintf(request_str,"GET %s %s",resource, HTTP_REQ_STR);
		request_str[strlen(request_str)] = '\0';
		printf("\nSending request :\n[%s]\n",request_str);
		return NULL;
	}
	n = write(client_socket, request_str, strlen(request_str));
	//int n = write(client_socket, str, strlen(str));
	if( n<0)
	{
		printf("\nError in sending request");
	}
	else
	{
		printf("\nSent request :\n[%s]\n",request_str);
	}
	free(request_str);
        received_file = fopen(HTML_FILE_LOCAL, "w");

        if (received_file == NULL)
        {
                fprintf(stderr, "Failed to open file foo\n");

                exit(EXIT_FAILURE);
        }
        /* Receiving file size */
	len = recv(client_socket, buffer, 512, 0);
	printf("\nMessage of length: %d\n\n",len);

	get_http_header(buffer, http_head, html_content);
	/*
	 * validate content type = txt/html in html_content->http_content_type
	 * */

	fwrite(html_content, sizeof(char), strlen(html_content), received_file);
	while(len > 0){
		len = recv(client_socket, buffer, 512, 0);
                fwrite(buffer, sizeof(char), len, received_file);
	}
        fclose(received_file);
	return NULL;
}
