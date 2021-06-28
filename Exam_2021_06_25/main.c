#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

#define START_TIME (7 * 60)
#define END_TIME (12 * 60)

#define TURN 30

#define YOUNG 20
#define MIDDLE 25

#define MAX_LENGTH 64

#define FILE_NAME "list.txt"

#define THREAD_SHARED 0

#define TRUE 1
#define FALSE 0

#define FAIL -1
#define SUCCESS 0

#define PDEATH(message)     \
    {                       \
        perror(message);    \
        exit(EXIT_FAILURE); \
    }

#define DEATH(message)         \
    {                          \
        printf("%s", message); \
        exit(EXIT_FAILURE);    \
    }


// global variables
sem_t sem_thread[3], sem_main;
char name[MAX_LENGTH];
char surname[MAX_LENGTH];
int current_time;
int finish;

//thread function
void *threadFunction(void *param) {
    int thread_num;

    // get thread number from param
    thread_num = *((int *) param);

    // print all the received people until main signal to finish
    while (!finish) {
        if (sem_wait(&sem_thread[thread_num]) == FAIL)
            PDEATH("failed to wait on semaphore thread\n")

        if (!finish) {
            // check if current_time there available current_time slots
            if (current_time < END_TIME) {
                // print new received name and current_time
                printf("%s %s %02d:%02d\n", name, surname, current_time / 60, current_time % 60);
                // increment current_time
                current_time += TURN;
            } else {
                printf("%s %s overbooking\n", name, surname);
            }
            if (sem_post(&sem_main) == FAIL)
                PDEATH("failed to post on semaphore main\n");
        }
    }

    // finish exit thread
    pthread_exit(EXIT_SUCCESS);
}

// main function
int main() {
    FILE *fp;
    pthread_t tid[3];
    int age, thread_num[3];

    // open file
    fp = fopen(FILE_NAME, "r");

    // init semaphore
    if (sem_init(&sem_main, THREAD_SHARED, 1) == FAIL)
        PDEATH("failed to init semaphore main\n");
    for (int i = 0; i < 3; i++) {
        if (sem_init(&sem_thread[i], THREAD_SHARED, 0) == FAIL)
            PDEATH("failed to init semaphore thread")
    }

    // create threads
    for (int i = 0; i < 3; i++) {
        thread_num[i] = i;
        if (pthread_create(&tid[i], NULL, threadFunction, (void *) &thread_num[i]) != SUCCESS)
            DEATH("failed to create thread\n")
    }

    // init current_time and finish variable
    current_time = START_TIME;
    finish = FALSE;

    // scan the content of the file
    while (!feof(fp)) {
        // wait until thread has finished printing
        if (sem_wait(&sem_main) == FAIL)
            PDEATH("failed to wait on semaphore main\n")

        // read new name, surname and age
        fscanf(fp, "%[^,],%[^,],%d\n", name, surname, &age);

        // select from age which thread to signal
        if (age <= YOUNG) {
            if (sem_post(&sem_thread[0]) == FAIL)
                PDEATH("failed to post on semaphore thread\n")
        } else if (age <= MIDDLE) {
            if (sem_post(&sem_thread[1]) == FAIL)
                PDEATH("failed to post on semaphore thread\n")
        } else {
            if (sem_post(&sem_thread[2]) == FAIL)
                PDEATH("failed to post on semaphore thread\n")
        }
    }
    // wait last time for the last person to be printed
    if (sem_wait(&sem_main) == FAIL)
        PDEATH("failed to wait on semaphore main\n")

    // finish, signal thread to exit, close file
    finish = TRUE;
    if (fclose(fp) == EOF)
        PDEATH("failed to close file\n")
    for (int i = 0; i < 3; i++) {
        if (sem_post(&sem_thread[i]) == FAIL)
            PDEATH("failed to post on semaphore thread\n")
    }

    // wait for thread to exit
    for (int i = 0; i < 3; i++) {
        if (pthread_join(tid[i], NULL) != SUCCESS)
            DEATH("failed to join thread\n")
    }

    // destroy all semaphores
    if (sem_destroy(&sem_main) == FAIL)
        PDEATH("failed to destroy semaphore main\n")
    for (int i = 0; i < 3; i++) {
        if (sem_destroy(&sem_thread[i]) == FAIL)
            PDEATH("failed to destroy semaphore thread\n")
    }
    return 0;
}
