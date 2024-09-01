#include "client_func.h"

int main(int argc, char const *argv[])
{
	// Input validation
	if (argc != 3)
	{
		printf("Invalid command! Use the below format:\n<executable code><Server IP Address><Server Port number>\n");
		exit(-1);
	}

	// basic declaraion
	struct sockaddr_in address;
	int client_sock;
	struct sockaddr_in server_addr;
	char buffer[MAX_BUFFER_SIZE];
	memset(buffer, 0, MAX_BUFFER_SIZE);

	// socket creation
	int PORT = atoi(argv[2]);
	const char* IP = argv[1];
	client_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (client_sock == -1) {
		perror("Failed to create a socket. Exiting!");
		exit(EXIT_FAILURE);
	}


	// server address configuration
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	if(inet_pton(AF_INET, IP, &server_addr.sin_addr)<=0) {
		perror("\nError: Invalid address. Address not supported. ");
		exit(EXIT_FAILURE);
		//error("\nError: Invalid address. Address not supported. ");
	}

	if (connect(client_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
		perror("\nError: Failed to connect to the remote host.");
		exit(EXIT_FAILURE);
		//error("\nError: Failed to connect to the remote host. ");
	}

	while(true){
		memset(buffer, 0, MAX_BUFFER_SIZE);
		fgets(buffer, MAX_BUFFER_SIZE, stdin);
		buffer[strcspn(buffer, "\n")] = 0;

		// decide for the correct operation from put/get/mget/mput
		char cmd_line[MAX_BUFFER_SIZE];
		strcpy(cmd_line,buffer);
		char* cmd = strtok(cmd_line," ");

		// If operation is put
		if (strcmp("PUT",cmd)==0)
		{
			int f = put_function(client_sock, cmd, buffer);
			if(f == -1)
			continue;
			
			else if(f == 1)
			break;		
		}
		else if (strcmp("GET",cmd)==0)
		{
			get_function(client_sock, cmd, buffer);
		}
		else if (strcmp("MPUT",cmd)==0)
		{
			mput_function(client_sock, cmd, buffer);
		}

		else if (strcmp("MGET",cmd)==0)
		{
			mget_function(client_sock, cmd, buffer);
		}
		
		else if (strcmp("EXIT",buffer)==0){
			send(client_sock, buffer, MAX_BUFFER_SIZE, 0);
			break;
		}
		else
		{
			printf("Invalid Command\n");
			continue;
		}
		printf("------Operation successfully completed using FTP-------\n\n");
		
	}
	close(client_sock);
	return 0;
}
