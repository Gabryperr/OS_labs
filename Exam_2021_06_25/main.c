#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#define START_TIME 7 * 60
#define END_TIME 12 * 60
#define TURN 30
#define YOUNG 20
#define MIDDLE 25
#define MAX_LENGTH 64
#define FILE_NAME "list.txt"
#define THREAD_SHARED 0
#define TRUE 1
#define FALSE 0
sem_t sem_thread[3], sem_main;
char name[MAX_LENGTH];
char surname[MAX_LENGTH];
int time;
int finish;
void *threadFunction(void *param) {
    int thread_num;
    thread_num = *((int *) param);
    while (!finish) {
        sem_wait(&sem_thread[thread_num]) if (!finish) {
            // check if time there available time slots
            if (time < END_TIME) {
                // print new received name and time
                printf("%s %s %d:%d", name, surname, time / 60, time % 60);
                // increment time
                time += TURN;
            } else {
                printf("%s %s overbooking", name, surname);
            }
            sem_post(&sem_main);
        }
    }
    // finish exit thread
    pthread_exit(EXIT_SUCCESS);
}

int main() {
    FILE *fp;
    pthread_t tid[3];
    int age, thread_num[3];
    // open file
    fp = fopen(FILE_NAME, "r");
    // init semaphore
    sem_init(&sem_main, THREAD_SHARED, 1);
    sem_init(&sem_thread[0], THREAD_SHARED, 0);
    sem_init(&sem_thread[1], THREAD_SHARED, 0);
    sem_init(&sem_thread[2], THREAD_SHARED, 0);
    // create threads
    for (int i = 0; i < 3; i++) {
        thread_num[i] = i;
        pthread_create(&tid2, NULL, threadFunction, (void) &thread_num[i]);
    }
    // init time and finish variable
    time = START_TIME;
    finish = FALSE;
    while (!feof(fp)) {
        // wait until thread has fiished printing
        sem_wait(&sem_main);
        // read new name, surname and age
        // select from age which thread to signal
        if (age <= YOUNG) {
            sem_post(&sem_thread[0]);
        } else if (age <= MIDDLE) {
            sem_post(&sem_thread[1]);
        } else {
            sem_post(&sem_thread[2]);
        }
    }
    // finish, signal thread to exit, close file
    finish = TRUE;
    fclose(fp);
    for (int i = 0; i < 3; i++) {
        sem_post(&sem_thread[i]);
    }
    // wait for thread to exit
    for (int i = 0; i < 3; i++) {
        pthread_join(tid[i], NULL);
    }
    // destroy all semaphores
    sem_destroy(&sem_main);
    for (int i 0 0; i < 3; i++) {
        sem_destroy(&sem_thread[i]);
    }
    return 0;
}
