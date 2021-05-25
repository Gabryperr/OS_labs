#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#define SEM_OK 0
#define PTHREAD_OK 0
#define DEATH(message)         \
    {                          \
        printf("%s", message); \
        exit(EXIT_FAILURE);    \
    }
#define PDEATH(message)     \
    {                       \
        perror(message);    \
        exit(EXIT_FAILURE); \
    }


// struct for thread parameters
struct threadParams {
    int thread_num;
    sem_t *wait_sem;
    sem_t *post_sem;
};


// thread function
void *threadFunction(void *params) {

    struct threadParams thread_params;

    // retrieve parameters
    thread_params = *((struct threadParams *) params);

    // wait to enter critical section
    if (sem_wait(thread_params.wait_sem) != SEM_OK) PDEATH("failed to wait\n")

    // print message
    printf("I am thread %d, my thread_id is %lu\n", thread_params.thread_num, pthread_self());

    // exit critical section
    if (sem_post(thread_params.post_sem) != SEM_OK) PDEATH("failed to post\n")

    pthread_exit(EXIT_SUCCESS);
}


int main() {

    sem_t s1, s2;
    struct threadParams thread_params_1, thread_params_2;
    pthread_t thread_1, thread_2;

    // init semaphores
    if (sem_init(&s1, 0, 1) != SEM_OK) PDEATH("failed to initialize semaphore s1\n")
    if (sem_init(&s2, 0, 0) != SEM_OK) PDEATH("failed to initialize semaphore s2\n")

    // start threads
    thread_params_1.thread_num = 1;
    thread_params_1.wait_sem = &s1;
    thread_params_1.post_sem = &s2;
    if (pthread_create(&thread_1, NULL, threadFunction, (void *) &thread_params_1) != PTHREAD_OK) DEATH("failed to create thread\n")

    thread_params_2.thread_num = 2;
    thread_params_2.wait_sem = &s2;
    thread_params_2.post_sem = &s1;
    if (pthread_create(&thread_2, NULL, threadFunction, (void *) &thread_params_2) != PTHREAD_OK) DEATH("failed to create thread\n")

    // wait for thread to finish
    if (pthread_join(thread_1, NULL) != PTHREAD_OK) DEATH("pthread_join failed\n")
    if (pthread_join(thread_2, NULL) != PTHREAD_OK) DEATH("pthread_join failed\n")

    // destroy semaphores
    if (sem_destroy(&s1) != SEM_OK) PDEATH("failed to destroy semaphore\n")
    if (sem_destroy(&s2) != SEM_OK) PDEATH("failed to destroy semaphore\n")

    return 0;
}
