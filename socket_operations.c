#include "socket_operations.h"


extern char* original_host;

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
//#ifdef VM_DEBUG_ON
//		localhost_server = gethostbyname("localhost");
//#else
		localhost_server = gethostbyname(crawling_host);
//#endif
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
int send_receive_socket_data(int client_socket, char* resource)
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
		len = HTTP_REQ_STR_LEN + strlen("GET /") + 1;
		memset(request_str,'\0',1024);
		sprintf(request_str,"GET / %s",HTTP_REQ_STR);
//		request_str[strlen(request_str)] = '\0';
//		fprintf( stderr,"\nSending request :\n[%s]\n",request_str);
	}
	else
	{
		len = HTTP_REQ_STR_LEN + strlen("GET /") + strlen(resource) + 1;
		memset(request_str,'\0',1024);
		sprintf(request_str,"GET /%s %s",resource, HTTP_REQ_STR);
//		request_str[strlen(request_str)] = '\0';
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
        /* Receiving file size */
	len = recv(client_socket, buffer, 1000, 0);
	//len =read(client_socket, buffer, sizeof(buffer)-1);
	fprintf( stderr,"\nResponse message of length: %d\n\n",len);
	
	//http_head = malloc(sizeof(Http_header) + 512);
	//
	memset(http_head.http_version,'\0',sizeof(http_head.http_version));
	memset(http_head.http_rsp_code,'\0',sizeof(http_head.http_rsp_code));
	memset(http_head.http_server,'\0',sizeof(http_head.http_server));
	memset(http_head.http_content_type,'\0',sizeof(http_head.http_content_type));
	http_head.http_content_length = 0;
	
	memset(html_content,'\0',1024);

	memset(deletethis,'\0',1024);
	strncpy(deletethis, buffer, len);

	get_http_header(buffer, &http_head, &html_content[0]);
//	fprintf(stderr,"\nhtml content is \n%s\n",html_content);
	
	int data_received = len;
	int html_data_received_initially = (len > http_head.http_content_length) ? 
		(len - http_head.http_content_length) : (http_head.http_content_length - len);

	fprintf(stderr,"\ndata_received = %d\nhtml_data_received_initially = %d\nhttp_head.http_content_length = %d\n",data_received,
		       	html_data_received_initially,http_head.http_content_length);


//	int data_remaining = http_head.http_content_length > html_data_received_initially ? (http_head.http_content_length - html_data_received_initially) : 0;


	int data_remaining = len > http_head.http_content_length? 0: http_head.http_content_length - len;
	fprintf(stderr,"\ndata_remaining = %d\n",data_remaining);


	if(http_head.http_version)
		fprintf(stderr,"\nversion is %s",http_head.http_version);
	if(http_head.http_rsp_code)
		fprintf(stderr,"\ncode is %s",http_head.http_rsp_code);
	if(http_head.http_server)
		fprintf(stderr,"\nServer is %s",http_head.http_server);
	if(http_head.http_content_type)
		fprintf(stderr,"\ncontent type is [%s]",http_head.http_content_type);
	
	int http_rsp_code = validate_rsp_code(http_head.http_rsp_code);
//	int success = 
	switch (http_rsp_code)
	{	
		case HTTP_RSP_503_SERVICE_UNAVAILABLE:
			fprintf(stderr,"\n-----+++++++++-----++++++service unavailable+------+++++------++\n[%s]",deletethis);
			return 2;//retry after
		case HTTP_RSP_200_SUCCESS:
			fprintf(stderr,"\n-----+++++++++-----++++++200 OK RESPONSE+------+++++------++\n");
			break;
		case HTTP_RSP_504_GW_TIMEOUT:
		case HTTP_RSP_414_REQ_URI_LONG:
		case HTTP_RSP_410_PERM_GONE:
		case HTTP_RSP_404_NOT_FOUND:
		default:
			fprintf(stderr,"\n-----+++++++++-----++++++page gone+------+++++------++\n");
			return 0;
	}
//#ifndef VM_DEBUG_ON
	if(strstr(http_head.http_content_type, MIME_TYPE_TEXT_HTML) == NULL)
	{
		fprintf( stderr,"\nContent type is not text/html, not reading more data---- RETURNING----\n");
		//dont parse data
		return 0;
	}
//#endif
	fprintf(stderr,"\nContent type and status valid.............\n");
	received_file = fopen(HTML_FILE_LOCAL, "w");
        if (received_file == NULL)
        {
		fprintf( stderr,"Failed to open local file");
		return 0;
        }
	int len1 = fwrite(html_content, sizeof(char), len, received_file);
	fprintf(stderr,"\n**** writing initial data to file of len %d****\n",len1);
	//while(data_remaining > 0)
	memset(buffer,'\0',1000);
	while(len > 0)
	{
		len = recv(client_socket, buffer, 512, 0);
		if(len < 0)
		{
			continue;
		}
		else
		{
			len1 +=	fwrite(buffer, sizeof(char), len, received_file);
		}
		data_remaining = data_remaining - len;
	}

	fprintf(stderr,"\n****finished writing file : added len %d more****\n",len1);
        fclose(received_file);
	return 1;
}
