#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include<errno.h>
#include <dirent.h>
#include<semaphore.h>



#define BUF_SIZE 1024
sem_t *m1, *m2;

void readFileAndWriteToPipe(int pipe_fd, const char *file_path) {
    FILE *file = fopen(file_path, "r");
    if (file) {
        fseek(file, 0L, SEEK_END);
        int file_size = ftell(file);
        rewind(file);

        char buffer[BUF_SIZE];
        fread(buffer, 1, file_size, file);
        fclose(file);

        write(pipe_fd, buffer, file_size);
    }else{
    printf("file not found");
}
}
int main()
{
    int p1[2], p2[2];
    pid_t pid1, pid2;
    char buffer[BUF_SIZE];

    m1 = sem_open("par_sem", O_CREAT, 0644, 0);
    m2 = sem_open("par_sem", O_CREAT, 0644, 0);
    
    // Create two pipes
  if (pipe(p1) == -1 || pipe(p2) == -1) {
    fprintf(stderr, "pipe error: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
}

    // Fork two child processes
    pid1 = fork();
   if (pid1 == -1) {
    fprintf(stderr, "fork error: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
}

    else if (pid1 == 0)
    {                 // First child process
        close(p1[0]); // Close unused read end of p1
        close(p2[0]); // Close unused read end of p2
        DIR *dir;
        struct dirent *ent;
        char dir_path[] = "d1"; // Change this to the path of the first directory
        if ((dir = opendir(dir_path)) != NULL)
        {
            while ((ent = readdir(dir)) != NULL)
            {
                char file_path[BUF_SIZE];
                snprintf(file_path, BUF_SIZE, "%s/%s", dir_path, ent->d_name);
                
                if(ent->d_name[0]=='.')
                   continue;

                    write(p1[1], ent->d_name,strlen(ent->d_name));
                    write(p1[1], ":", 1);
                     readFileAndWriteToPipe(p1[1],file_path);
                     write(p1[1], ":", 1);
            }
            closedir(dir);
        }
      else {
    fprintf(stderr, "opendir error: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
}

        sem_post(m1);
      //  printf("exiting from child 1 process\n");

        close(p1[1]); // Close write end of p1
        exit(EXIT_SUCCESS);
    }

    else
    { // Parent process
        pid2 = fork();
       if (pid2 == -1) {
    fprintf(stderr, "fork error: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
}

        else if (pid2 == 0)
        {                 // Second child process
            close(p1[0]); // Close unused read end of p1
            close(p2[0]); // Close unused read end of p2
            DIR *dir;
            struct dirent *ent;
            char dir_path[] = "d2"; // Change this to the path of the second directory
            if ((dir = opendir(dir_path)) != NULL)
            {
                while ((ent = readdir(dir)) != NULL)
                {
                    char file_path[BUF_SIZE];
                    snprintf(file_path, BUF_SIZE, "%s/%s", dir_path, ent->d_name);

                    if(ent->d_name[0]=='.')
                   continue;

                       write(p2[1], ent->d_name, strlen(ent->d_name));
                      write(p2[1], ":", 1);
                    readFileAndWriteToPipe(p2[1],file_path);
                     write(p2[1], ":", 1);
   
                }
                closedir(dir);
            }
           else {
    fprintf(stderr, "opendir error: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
}
            sem_post(m2);

            close(p2[1]); // Close write end of p2
            exit(EXIT_SUCCESS);
        }

        // Parent process

       sem_wait(m1);
        sem_wait(m2);

        close(p1[1]); // Close unused write end of p1
        close(p2[1]); // Close unused write end of p2
        char buf1[BUF_SIZE], buf2[BUF_SIZE];
        int nbytes1 = read(p1[0], buf1, BUF_SIZE);

      if (nbytes1 == -1) {
    fprintf(stderr, "read error: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
}

        int nbytes2 = read(p2[0], buf2, BUF_SIZE);
       if (nbytes2 == -1) {
    fprintf(stderr, "read error: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
}

        buf1[nbytes1] = '\0';
        buf2[nbytes2] = '\0';
        if (nbytes1 > 0 && nbytes2 > 0)
        {
         
            printf("Child1:\n");
            char* token1,*rest=buf1,*token2;

             while ((token1 = strtok_r(rest, ":", &rest)))
               {
                 token2 = strtok_r(rest, ":", &rest);

                   printf("%s: %s\n",token1,token2);
                 char file_name[50];
                  sprintf(file_name,"d2/%s",token1);

                   FILE* fp1=fopen(file_name,"w");
                   if(fp1==NULL) { printf("%s file not found",token1); continue;}

                   fprintf(fp1,"%s",token2);
                   fclose(fp1);
               }
            
             printf("\n\nChild2:\n");
             token1=NULL;token2=NULL;rest=buf2;
             
             while ((token1 = strtok_r(rest, ":", &rest)))
               {
                 token2 = strtok_r(rest, ":", &rest);

                   printf("%s: %s\n",token1,token2);
                 char file_name[50];
                  sprintf(file_name,"d1/%s",token1);

                   FILE* fp1=fopen(file_name,"w");
                   if(fp1==NULL) {
                     printf("%s file does not exist",token1);
                   
                    continue;
                    }

                   fprintf(fp1,"%s",token2);
                   fclose(fp1);
               }
        }
    else if (nbytes1 > 0) {
    printf("Child 1 successfully read data:\n%s\n\n", buf1);
    printf("Error: Child 2 encountered an error.\n");
}
else if (nbytes2 > 0) {
    printf("Error: Child 1 encountered an error.\n");
    printf("Child 2 successfully read data:\n%s\n", buf2);
}
else {
    printf("Error: Both child processes encountered errors.\n");
}

        sem_post(m1);
        sem_post(m2);

        wait(NULL);
        wait(NULL);

        sem_unlink("par_sem");  //semaphores are destroyed here

        exit(EXIT_SUCCESS);
    }
}
