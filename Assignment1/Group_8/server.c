#include "serv_func.h"


int main(int argc, char const *argv[])
{
	// Input Validation
	if (argc != 2){
		printf("Invalid command! Use the below format:\n<executable code> <Server Port number>\n");
		exit(-1);
	}

	// Basic Declarations for socket programming
	int PORT;
	int server_sock;
	struct sockaddr_in client_addr;
	char buffer[MAX_BUFFER_SIZE];
	memset(buffer, 0, MAX_BUFFER_SIZE);

	// Create a socket for communication
	PORT = atoi(argv[1]);
	if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Failed to create a socket. Exiting!");
        exit(EXIT_FAILURE);
    }


	// configure client address details
	client_addr.sin_family = AF_INET;
	client_addr.sin_addr.s_addr = INADDR_ANY;
	client_addr.sin_port = htons(PORT);

	// Bind the socket to the application
	if (bind(server_sock, (struct sockaddr*)&client_addr, sizeof(client_addr)) == -1) {
        perror("Binding failed. Exiting!");
        exit(EXIT_FAILURE);
    }


	// Start listening to the port
	if (listen(server_sock, MAX_CONNECTIONS) == -1) {
        perror("Unable to listen to the port");
        exit(EXIT_FAILURE);
    }


	// printf("fo\n");
	
	// Create a loop for accepting connections
	while(true){
		int accept_sockfd;
		int addr_len = sizeof(client_addr);
        printf("Waiting for connection...\n");

		accept_sockfd = accept(server_sock, (struct sockaddr*)&client_addr, (socklen_t*)&addr_len);
        if (accept_sockfd == -1) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        printf("Received connection from client!\n");

		
		while(true){
            
			// receive command from user
			recv(accept_sockfd, buffer, MAX_BUFFER_SIZE, 0);
			// printf("%s",buffer );
			// if (buffer)
			// {
			// 	break;
			// }
			char cmd_line[MAX_BUFFER_SIZE];
			strcpy(cmd_line,buffer);
			char* cmd = strtok(cmd_line," ");
			printf("%s\n",cmd );
			if (strcmp(cmd,"EXIT")==0){
				printf("Exiting\n");
				break;
			}
			else if (strcmp(cmd,"GET")==0)
			{
                get_function(accept_sockfd, cmd);
  			}
			else if (strcmp(cmd,"PUT")==0)
			{
				put_function(accept_sockfd, cmd, buffer);
			}
			else if (strcmp(cmd,"MGET")==0)
			{
                mget_function(accept_sockfd, cmd, buffer);
			}
			else{
				printf("\nInvalid\n");
			}
			
			memset(buffer, 0, MAX_BUFFER_SIZE);
		}
				

    	close(accept_sockfd);
        printf("Connection closed\n");
		
	}
	close(server_sock);


	return 0;
}

