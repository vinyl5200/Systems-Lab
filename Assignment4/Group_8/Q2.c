#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <time.h>
#include <wait.h>


struct message_buff {
    long mtype;
    char mtext[100];
};

int main() {
    int message_id_pc, message_id_cp;
    int process_id;
    struct message_buff msg;
    key_t keypc, keycp;
    int number_of_child = 0;
    int number_of_ques = 0;
    char answer[4] = {'a','b','c','d'}; // outcomes that are possible
    

    printf("Please provide the number of students : ");
    scanf("%d", &number_of_child);

    printf("\nPlease provide number of questions : ");
    scanf("%d", &number_of_ques);

    if(number_of_ques<=0 || number_of_child<=0) // checks whether inputs provided are valid or not
    {
        printf("\nInputs provided are not valid ...!!\n\n");
        return -1;
    }

    char *answers = malloc(sizeof(char)*number_of_ques);

    
    for(int i = 0; i < number_of_ques; i++) //Randomly generating corr_ans answers for questions
    {
        answers[i] = answer[rand()%4];
    }

    printf("\n");

    // Generate a key for the message queue
    keypc = ftok(".", 'a');
    keycp = ftok(".", 'c');

    // Creating  two message queues
    message_id_pc = msgget(keypc, 0666 | IPC_CREAT);
    message_id_cp = msgget(keycp, 0666 | IPC_CREAT);


    if (message_id_pc == -1 || message_id_cp == -1) {
        perror("msgget");
        exit(1);
    }

    for (int i = 0; i < number_of_child; i++) {
        process_id = fork();

        if (process_id == -1) {
            perror("fork");
            exit(1);
        } else if (process_id == 0) {

            srand(time(NULL) + i);
            char *child_answers = (char*)malloc(sizeof(char)*number_of_ques);
            
            // Child process receives a message from parent
            for(int j = 0; j < number_of_ques; j++) {
                if (msgrcv(message_id_pc, &msg, sizeof(msg.mtext), i+1, 0) == -1) {
                    perror("msgrcv");
                    exit(1);
                }
                child_answers[j] = answer[rand()%4];
            }
                
            msg.mtype = i + 1;
            
            //concatenating responses 
            sprintf(msg.mtext,"%s",child_answers);

            //sending responses to parent
            if (msgsnd(message_id_cp, &msg, sizeof(msg.mtext), 0) == -1) {
                perror("msgsnd");
                exit(1);
            }
            
            // child process receiving grades
            if (msgrcv(message_id_pc, &msg, sizeof(msg.mtext), i+1, 0) == -1) {
                perror("msgrcv");
                exit(1);
            }

            //printf("\tStudent : %d \tGrade %s \n", i+1, msg.mtext);
            printf("\tGrade of Student %d is : %s \n", i+1, msg.mtext);
            exit(0);
        }
    }

    
    char question[] = "Select the best option possible from (a,b,c,d)";

    printf("**************************************************************\n");
    printf("*                    Student Grade Distribution               *\n");
    printf("**************************************************************\n\n");


    // Parent process sends a message to each child
    for (int i = 0; i < number_of_child; i++) {

        for(int j = 0; j < number_of_ques; j++){
            sprintf(msg.mtext, question, i+1);
            msg.mtype = i+1;
            if (msgsnd(message_id_pc, &msg, sizeof(msg.mtext), 0) == -1) {
                perror("msgsnd");
                exit(1);
            }
        }
    }

    // Parent process receives a message from each child
    int score[number_of_child];
    int Total_grade = 0;
    for (int i = 0; i < number_of_child; i++) {
        int corr_ans = 0;
        if (msgrcv(message_id_cp, &msg, sizeof(msg.mtext), i + 1, 0) == -1) {
            perror("msgrcv");
            exit(1);
        }

        //calculating score
        for(int j = 0; j < number_of_ques; j++){
            if(msg.mtext[j] == answers[j]){
                corr_ans++;
            }
        }
        score[i] = corr_ans;
    }

    int max_score = 0;
    for(int i = 0; i < number_of_child; i++){
        if(max_score < score[i]){
            max_score = score[i];
        }
    }

    //calculating the final scores
    long gradeA,gradeB,gradeC,gradeD;
    gradeA = max_score - (0.1*max_score);
    gradeB = max_score - (0.2*max_score);
    gradeC = max_score - (0.3*max_score);
    gradeD = max_score - (0.4*max_score);

    char grade[number_of_child];

    //calculating grades
    for(int i = 0; i < number_of_child; i++){
        if(score[i] >= gradeA){
            grade[i] = 'A';
            Total_grade += 75;
        }else if(score[i] >= gradeB && score[i] < gradeA){
            grade[i] = 'B';
            Total_grade += 60;
        }else if(score[i] >= gradeC && score[i] < gradeB){
            grade[i] = 'C';
            Total_grade += 45;
        }else if(score[i] >= gradeD && score[i] < gradeC){
            grade[i] = 'D';
            Total_grade += 33;
        }else{
            Total_grade += 0;
            grade[i] = 'F';
        }
    }

    //parent sending grades to child processes
    for (int i = 0; i < number_of_child; i++) {

        sprintf(msg.mtext,"%c",grade[i]);
        msg.mtype = i+1;
        if (msgsnd(message_id_pc, &msg, sizeof(msg.mtext), 0) == -1) {
            perror("msgsnd");
            exit(1);
        }
    }

    for (int i = 0; i < number_of_child; i++) {
        wait(NULL);
    }

    double avg_percentage = (double)Total_grade/number_of_child;

    printf("\n\n**************************************************************\n");
    printf("\n\tOverall Percentage Distribution: %.3lf %%",avg_percentage);
    printf("\n\n**************************************************************\n");



    // Delete message queue
    if (msgctl(message_id_cp, IPC_RMID, NULL) == -1 || msgctl(message_id_pc, IPC_RMID, NULL) == -1) {
        perror("msgctl");
        exit(1);
    }
    return 0;
}
