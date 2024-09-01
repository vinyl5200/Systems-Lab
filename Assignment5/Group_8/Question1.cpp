#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

int NUM;					// number of people waiting outside restaurant
int N;						// number of diners in each batch
    
// semaphore declarations
sem_t mutex_enter, mutex_exit, entering_diners, exiting_diners, enter_done, exit_done;

int count_enter = 0, count_exit = 0;

void *restaurant(void *arg) {

    while (1) {  // Run indefinitely
        printf("\nRestaurant is preparing for a batch.\n");
        
        // open front door
        printf("Restaurant opened its front door.\n");
        
        // let diners in
        for (int j = 0; j < N; j++) {
            sem_post(&entering_diners);  // signal to a diner that they can enter
        }
        
        // wait until all diners have entered
        sem_wait(&enter_done);
        
        // close front door
        printf("Restaurant closed its front door.\n");
        
        // serve food to diners
        printf("Restaurant is serving the diners.\n");
        sleep(3); // simulate serving time
        
        // open back door
        printf("Restaurant opened its back door.\n");
        
        // let diners out
        for (int j = 0; j < N; j++) {
            sem_post(&exiting_diners);  // signal to a diner that they can exit
        }
        
        // wait until all diners have exited
        sem_wait(&exit_done);
        
        // close back door
        printf("Restaurant closed its back door.\n");
    }
}

void *diner(void *arg) {
	while(1) {
	    // Generate random sleep time
	    srand(time(NULL));
	    int sleep_time = rand() % 10 + 1; // Random sleep time between 1 and 10 microseconds

	    // Sleep for random duration
	    usleep(sleep_time);
	    
	    // enter restaurant
	    sem_wait(&entering_diners);  // wait until restaurant signals that diner can enter
	    printf("Diner %d entered the restaurant.\n", *(int *)arg);
	    
	    // check if all diners have entered
	    sem_wait(&mutex_enter);  // acquire mutex to update count_enter
	    count_enter++;
	    if (count_enter == N) {
		sem_post(&enter_done);  // signal to restaurant that all diners have entered
		count_enter = 0;
	    }
	    sem_post(&mutex_enter);  // release mutex
	    
	    sleep(1);  // simulate time to get seated and look at the menu
	    
	    // eat food
	    printf("Diner %d is eating.\n", *(int *)arg);
	    sleep(2); // simulate eating time
	    
	    // exit restaurant
	    sem_wait(&exiting_diners);  // wait until restaurant signals that diner can exit
	    printf("Diner %d exited the restaurant.\n", *(int *)arg);
	    
	    // check if all diners have exited
	    sem_wait(&mutex_exit);  // acquire mutex to update count_exit
	    count_exit++;
	    if (count_exit == N) {
		sem_post(&exit_done);  // signal to restaurant that all diners have exited
		count_exit = 0;
	    }
	    sem_post(&mutex_exit);  // release mutex
	}
}
// start of main function
int main() {
    
    printf("\nEnter the number of people waiting outside the restaurant: ");
    scanf("%d", &NUM);
    
    printf("\nEnter the number of people that the restaurant can serve (N): ");
    scanf("%d", &N);
	
    //handling the case when restaurant capacity is zero
    
    /*if(N==0){
    	printf("Exiting because restaurant capacity can't be zero\n");
    	return 0;
    }*/
    // declare variables
    pthread_t restaurant_thread, diner_threads[NUM];
    
    // initialize semaphores
    sem_init(&mutex_enter, 0, 1); // binary semaphore for mutual exclusion during entering the restaurant
    sem_init(&mutex_exit, 0, 1); // binary semaphore for mutual exclusion during exiting the restaurant
    sem_init(&entering_diners, 0, 0); // counting semaphore to keep track of diners entering the restaurant
    sem_init(&exiting_diners, 0, 0); // counting semaphore to keep track of diners exiting the restaurant
    sem_init(&enter_done, 0, 0); // binary semaphore to signal when entering the restaurant is done
    sem_init(&exit_done, 0, 0); // binary semaphore to signal when exiting the restaurant is done
    
    // create restaurant thread
    pthread_create(&restaurant_thread, NULL, restaurant, NULL);
    
    // Create diner threads with random scheduling
    for (int i = 0; i < NUM; i++) {

        
        int *arg = new int;
        *arg = i + 1; // Set the diner ID as the argument
        
        // Create diner thread with random scheduling
        pthread_create(&diner_threads[i], NULL, diner, arg);
    }
	
	// wait for all diner threads to finish
	for (int i = 0; i < NUM; i++) {
	    pthread_join(diner_threads[i], NULL); // wait for diner thread to finish
	}
	
	// wait for restaurant thread to finish
	pthread_join(restaurant_thread, NULL); // wait for restaurant thread to finish

	// destroy semaphores
	sem_destroy(&mutex_enter); // destroy the binary semaphore for mutual exclusion during entering the restaurant
	sem_destroy(&mutex_exit); // destroy the binary semaphore for mutual exclusion during exiting the restaurant
	sem_destroy(&entering_diners); // destroy the counting semaphore to keep track of diners entering the restaurant
	sem_destroy(&exiting_diners); // destroy the counting semaphore to keep track of diners exiting the restaurant
	sem_destroy(&enter_done); // destroy the binary semaphore to signal when entering the restaurant is done
	sem_destroy(&exit_done); // destroy the binary semaphore to signal when exiting the restaurant is done

	return 0; // exit program with return code 0
}



