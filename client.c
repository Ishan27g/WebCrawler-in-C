/* Client code */
/* TODO : Modify to meet your need */
#include "client.h"
#include "file_parser.h"
#include "socket_operations.h"

char* original_host;

int main(int argc, char **argv)
{

	int client_socket;

	original_host = NULL;
	
	if(argv[1] == NULL)
	{
		printf("\nEnter hostname as 1st arguement\n");
	}
	original_host = (char*) calloc(strlen(argv[1]), sizeof(char));
	strcpy(original_host, argv[1]);
	printf("\nhost is %s\n",original_host);

	client_socket = initialise_socket();

	send_receive_socket_data(client_socket, NULL);
        close(client_socket);
	parse_html_file(HTML_FILE_LOCAL, client_socket);

        return 0;
}

