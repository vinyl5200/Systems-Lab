#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h> 

#define MAX_BUFFER_SIZE 512
#define MAX_CONNECTIONS 3
#define DSK "./server_disk/"


int fileExistsInDirectory(char *filename) {
    DIR *dir;
    struct dirent *entry;

    dir = opendir(DSK);
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
    strcpy(file_path, DSK);
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
    strcpy(file_path, DSK);  
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
    printf("Received file from client!\n");
    fclose(file);

    return 0;
}

void put_file(int accept_sockfd, char* fname) {
    char buffer[MAX_BUFFER_SIZE];
    
    if (fileExistsInDirectory(fname)) {
        printf("File already exists!\n");
        printf("Do you want to overwrite the file? (yes/no)\n");
        memset(buffer, 0, MAX_BUFFER_SIZE);
        strcpy(buffer, "CONTINUE");
        send(accept_sockfd, buffer, MAX_BUFFER_SIZE, 0);

        memset(buffer, 0, MAX_BUFFER_SIZE);
        recv(accept_sockfd, buffer, MAX_BUFFER_SIZE, 0);
        //printf("Response: %s\n", buffer);

        if (strcmp(buffer, "yes") == 0) {
            memset(buffer, 0, MAX_BUFFER_SIZE);
            strcpy(buffer, "OKAY");
            send(accept_sockfd, buffer, MAX_BUFFER_SIZE, 0);
            receive_file_content(accept_sockfd, fname);
            printf("File received successfully\n\n");
        } else {
            memset(buffer, 0, MAX_BUFFER_SIZE);
            strcpy(buffer, "ABORT");
            send(accept_sockfd, buffer, MAX_BUFFER_SIZE, 0);
            printf("Aborted\n\n");
        }
    } else {
        memset(buffer, 0, MAX_BUFFER_SIZE);
        strcpy(buffer, "OKAY");
        send(accept_sockfd, buffer, MAX_BUFFER_SIZE, 0);
        receive_file_content(accept_sockfd, fname);
    }
}

void get_file(int accept_sockfd, char* fname) {
    char buffer[MAX_BUFFER_SIZE];
    
    if (fileExistsInDirectory(fname)) {
        printf("File already exists !\n");
        memset(buffer, 0, MAX_BUFFER_SIZE);
        strcpy(buffer, "READY");
        send(accept_sockfd, buffer, MAX_BUFFER_SIZE, 0);

        recv(accept_sockfd, buffer, MAX_BUFFER_SIZE, 0);

        if (strcmp(buffer, "yes") == 0) {
            send_file_content(accept_sockfd, fname);
        } else {
            memset(buffer, 0, MAX_BUFFER_SIZE);
            strcpy(buffer, "ABORT");
            send(accept_sockfd, buffer, MAX_BUFFER_SIZE, 0);
            printf("Aborted\n");
        }
        printf("File sent successfully!\n\n");
    } else {
        memset(buffer, 0, MAX_BUFFER_SIZE);
        strcpy(buffer, "CANCEL");
        send(accept_sockfd, buffer, MAX_BUFFER_SIZE, 0);
    }
}

void get_function(int accept_sockfd, char* cmd)
{
    char *fname;
	cmd =  strtok(NULL," ");
	fname = cmd;
	get_file(accept_sockfd, fname);
}
void put_function(int accept_sockfd, char* cmd, char* buffer)
{
    char *fname;
	cmd =  strtok(NULL," ");
	fname = cmd;
        put_file(accept_sockfd, fname);
        memset(buffer, 0, MAX_BUFFER_SIZE);
        strcpy(buffer,"SUCCESS");
        send(accept_sockfd, buffer, MAX_BUFFER_SIZE, 0);
}
void mget_function(int accept_sockfd, char* cmd, char* buffer)
{
    char *fext, *fname;
                cmd =  strtok(NULL," ");
                fext = cmd;
                printf("Sending all %s files\n",fext );
				DIR *di;
                struct dirent *dir;
                di = opendir(DSK);
                memset(buffer, 0, MAX_BUFFER_SIZE);
                while ((dir = readdir(di)) != NULL){
                    fname= dir->d_name;
                    // printf("--file : %s\n", fname);
                    char* ext = strrchr(fname, '.');
                    if (ext==NULL)
                    {
                        continue;
                    }
                    if (strcmp(ext,fext)==0)
                    {
                        
                        memset(buffer, 0, MAX_BUFFER_SIZE);
                        strcpy(buffer,fname);
                        printf("%s to be sent to client\n",buffer );
                        send(accept_sockfd, buffer, MAX_BUFFER_SIZE, 0);
                        memset(buffer, 0, MAX_BUFFER_SIZE);
                        
                        recv(accept_sockfd, buffer, MAX_BUFFER_SIZE, 0);
                        printf("%s\n",buffer );
                        if (strcmp(buffer,"SKIP")==0)
                        {
                            continue;
                        }
                        else if (strcmp(buffer,"SEND")==0)
                        {
                            memset(buffer, 0, MAX_BUFFER_SIZE);
                            printf("Sending file %s to Client's Disk\n\n",fname );
                            send_file_content(accept_sockfd, fname);
                        }

                        memset(buffer, 0, MAX_BUFFER_SIZE);
                        recv(accept_sockfd, buffer, MAX_BUFFER_SIZE, 0);
                        if (strcmp(buffer,"READY"))
                        {
                            printf("Error!\n");
                            break;
                        }
  
                    }
                }
                
                memset(buffer, 0, MAX_BUFFER_SIZE);
                strcpy(buffer,"OVER");
                send(accept_sockfd, buffer, MAX_BUFFER_SIZE, 0);
                memset(buffer, 0, MAX_BUFFER_SIZE);
                recv(accept_sockfd, buffer, MAX_BUFFER_SIZE, 0);
                printf("Sent all files of %s\n",fext );

                closedir(di);
}
