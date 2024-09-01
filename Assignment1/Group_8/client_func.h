#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dirent.h> 
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>


#define MAX_BUFFER_SIZE 512
#define CNTD "./client_disk/"


int fileExistsInDirectory(char *filename) {
    DIR *dir;
    struct dirent *entry;

    dir = opendir(CNTD);
    if (dir == NULL) {
        perror("Error opening directory");
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, filename) == 0) {
            closedir(dir);
            return 1;  // File found
        }
    }

    closedir(dir);
    return 0;  // File not found
}

int send_file_content(int socketDescriptor, char* file_name){

    char file_path[256];
    strcpy(file_path, CNTD);
    strcat(file_path, file_name);

    // Open the file in binary mode
    FILE *file = fopen(file_path, "rb");
    if (!file) {
        perror("Error opening file");
        return -1;
    }

    // Read file content into buffer and send it
    char buffer[MAX_BUFFER_SIZE];
    size_t bytesRead;
    memset(buffer, 0, MAX_BUFFER_SIZE);
    
    while ((bytesRead = fread(buffer, 1, MAX_BUFFER_SIZE, file)) > 0) {
        if (send(socketDescriptor, buffer, bytesRead, 0) == -1) {
            perror("Error sending file content");
        }
        memset(buffer, 0, MAX_BUFFER_SIZE);
    }
    
    // Close the file
    fclose(file);
    
    return 0;
}

int receive_file_content(int socketDescriptor, char* file_name) {

    char file_path[256];
    strcpy(file_path, CNTD);  
    strcat(file_path, file_name);

    // Open the file in binary mode for writing
    FILE *file = fopen(file_path, "wb");
    if (!file) {
        perror("Error opening file for writing");
        return -1;
    }

    // Receive file content from the client and write it to the file
    char buffer[MAX_BUFFER_SIZE];
    size_t bytesReceived;

    do {
        bytesReceived = recv(socketDescriptor, buffer, sizeof(buffer), 0);
        if (bytesReceived == -1) {
            perror("Error receiving file content");
            fclose(file);
            return -1;
        }

        // Write received data to the file
        fwrite(buffer, 1, bytesReceived, file);

        // If bytesReceived is less than buffer size, it means end of file
    } while (bytesReceived == sizeof(buffer));


    // Close the file
    printf("Received file from server!\n");
    fclose(file);

    return 0;
}


void put_file(int client_sock, char *fname) {
    char buffer[MAX_BUFFER_SIZE];
    memset(buffer, 0, MAX_BUFFER_SIZE);
    recv(client_sock, buffer, MAX_BUFFER_SIZE, 0);

    if (strcmp(buffer, "ABORT") == 0) {
        printf("Aborted!\n");
    }

    if (strcmp(buffer, "OKAY") == 0) {
        send_file_content(client_sock, fname);
    } else {
        printf("File already present!\nDo you want to overwrite file %s? (yes/no)\n", fname);
        memset(buffer, 0, MAX_BUFFER_SIZE);
        fgets(buffer, MAX_BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = 0;
        send(client_sock, buffer, MAX_BUFFER_SIZE, 0);

        if (strcmp(buffer, "yes") == 0) {
            memset(buffer, 0, MAX_BUFFER_SIZE);
            recv(client_sock, buffer, MAX_BUFFER_SIZE, 0);

            if (strcmp(buffer, "OKAY") == 0) {
                send_file_content(client_sock, fname);
                memset(buffer, 0, MAX_BUFFER_SIZE);
            } else {
                printf("Aborted!\n");
            }

            printf("File sent successfully!\n\n");
        } else {
            memset(buffer, 0, MAX_BUFFER_SIZE);
            recv(client_sock, buffer, MAX_BUFFER_SIZE, 0);

            if (strcmp("ABORT", buffer) == 0) {
                printf("Unexpected response from server\n");
            }

            printf("Aborted!\n\n");
        }
    }
}


void get_file(int client_sock, char* fname) {
    char buffer[MAX_BUFFER_SIZE];
    memset(buffer, 0, MAX_BUFFER_SIZE);
    recv(client_sock, buffer, MAX_BUFFER_SIZE, 0);

    if (strcmp(buffer, "ABORT") == 0) {
        printf("Aborted!\n");
    } else if (strcmp(buffer, "READY") == 0) {
        if (fileExistsInDirectory(fname)) {
            printf("Do you want to overwrite the file (yes/no) ? ");
            memset(buffer, 0, MAX_BUFFER_SIZE);
            fgets(buffer, MAX_BUFFER_SIZE, stdin);
            buffer[strcspn(buffer, "\n")] = 0;
            send(client_sock, buffer, MAX_BUFFER_SIZE, 0);

            if (strcmp(buffer, "yes") == 0) {
                receive_file_content(client_sock, fname);
            } else {
                //printf("Sending 'no' to the server\n");
                memset(buffer, 0, MAX_BUFFER_SIZE);
                recv(client_sock, buffer, MAX_BUFFER_SIZE, 0);

                if (strcmp("ABORT", buffer) == 0) {
                    printf("Unexpected response from server\n");
                }

                printf("Aborted!\n");
            }
        } else {
            memset(buffer, 0, MAX_BUFFER_SIZE);
            strcpy(buffer, "yes");
            send(client_sock, buffer, MAX_BUFFER_SIZE, 0);
            memset(buffer, 0, MAX_BUFFER_SIZE);
            receive_file_content(client_sock, fname);
            memset(buffer, 0, MAX_BUFFER_SIZE);
            printf("File received successfully.\n");
        }
    } else {
        printf("File %s not found.\n", fname);
    }
}

int put_function(int client_sock, char* cmd, char* buffer)
{
	// check if file exist
			char *fname;
			cmd =  strtok(NULL," ");
			fname = cmd;
			if (!fileExistsInDirectory(fname)){
				printf("ERROR: File %s not found.\n", fname);
				return -1;
			}

			// send command to server
			printf("CLIENT : %s is being transferred to server.\n",buffer );
			send(client_sock, buffer, MAX_BUFFER_SIZE, 0);
			put_file(client_sock, fname);
			memset(buffer, 0, MAX_BUFFER_SIZE);
            recv(client_sock, buffer, MAX_BUFFER_SIZE, 0);
            if (strcmp(buffer,"SUCCESS")){
            	printf("Facing issue while sending file. Please try again\n");
            	return 1;
            }
            return 0;
}

void get_function(int client_sock, char* cmd, char* buffer)
{
	// check if file exist
			char *fname;
			cmd =  strtok(NULL," ");
			fname = cmd;

			// send command to server
			send(client_sock, buffer, MAX_BUFFER_SIZE, 0);

			// Check for server reponse
			get_file(client_sock, fname);
}

void mput_function(int client_sock, char* cmd, char* buffer)
{
	// send(client_sock, buffer, MAX_BUFFER_SIZE, 0);
			
			char *fext, *fname;
			cmd =  strtok(NULL," ");
			fext = cmd;
			printf("Sending all %s files\n\n",fext );
			DIR *di;
			struct dirent *dir;
			di = opendir(CNTD);
			while ((dir = readdir(di)) != NULL){
				fname= dir->d_name;
				char* ext = strrchr(fname, '.');
				if (strcmp(ext,fext)==0)
				{
					memset(buffer, 0, MAX_BUFFER_SIZE);
					strcpy(buffer,"PUT ");
					strcat(buffer,fname);
					printf("%s is being send to server\n",buffer );
					send(client_sock, buffer, MAX_BUFFER_SIZE, 0);
					put_file(client_sock,fname);
					memset(buffer, 0, MAX_BUFFER_SIZE);
                    recv(client_sock, buffer, MAX_BUFFER_SIZE, 0);
                    if (strcmp(buffer,"SUCCESS"))
                    {
                    	printf(".....Error!!....\n");
                    	break;
                    }


				}
			}
			closedir(di);
}

void mget_function(int client_sock, char* cmd, char* buffer)
{
		char *fext, *fname;
			cmd =  strtok(NULL," ");
			fext = cmd;
			printf("Getting all %s files\n",fext );

			// send command to server
			
			send(client_sock, buffer, MAX_BUFFER_SIZE, 0);
			while(1){
				memset(buffer, 0, MAX_BUFFER_SIZE);
				recv(client_sock, buffer, MAX_BUFFER_SIZE, 0);
				if (strcmp(buffer,"OVER")!=0)
				{
					char fname[MAX_BUFFER_SIZE];
					strcpy(fname, buffer);
					// printf("%s\n",buffer );

					if (fileExistsInDirectory(fname))
					{
						printf("Do you want to overwrite the file %s (yes/no) ? ",fname);
						memset(buffer, 0, MAX_BUFFER_SIZE);
						fgets(buffer, MAX_BUFFER_SIZE, stdin);
						buffer[strcspn(buffer, "\n")] = 0;
						if (strcmp(buffer,"yes")==0)
						{
							memset(buffer, 0, MAX_BUFFER_SIZE);
							strcpy(buffer, "SEND");
							send(client_sock, buffer, MAX_BUFFER_SIZE, 0);

							memset(buffer, 0, MAX_BUFFER_SIZE);
							printf("Receiving file content for %s\n",fname );
							receive_file_content(client_sock,fname);

							memset(buffer, 0, MAX_BUFFER_SIZE);
	                        strcpy(buffer, "READY");
	                        send(client_sock, buffer, MAX_BUFFER_SIZE, 0);
						}
						else{
							memset(buffer, 0, MAX_BUFFER_SIZE);
							strcpy(buffer, "SKIP");
							send(client_sock, buffer, MAX_BUFFER_SIZE, 0);
						}
					}
					else{
						memset(buffer, 0, MAX_BUFFER_SIZE);
						strcpy(buffer, "SEND");
						send(client_sock, buffer, MAX_BUFFER_SIZE, 0);

						memset(buffer, 0, MAX_BUFFER_SIZE);
						printf("Receiving file content for %s\n",fname );
						receive_file_content(client_sock,fname);

						memset(buffer, 0, MAX_BUFFER_SIZE);
                        strcpy(buffer, "READY");
                        send(client_sock, buffer, MAX_BUFFER_SIZE, 0);
					}

				}
				else{
					printf("All files received\n");
					memset(buffer, 0, MAX_BUFFER_SIZE);
					strcpy(buffer, "DONE");
					send(client_sock, buffer, MAX_BUFFER_SIZE, 0);
					break;
				}
			}
}


