#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define THREAD_NUM 3
#define PTHREAD_OK 0
#define SEM_OK 0
#define TRUE 1
#define FALSE 0
#define DEATH(message)         \
    {                          \
        printf("%s", message); \
        exit(EXIT_FAILURE);    \
    }

pthread_mutex_t mutex;
pthread_cond_t cond_var[THREAD_NUM];
int finish[THREAD_NUM];

void *threadFunction(void *params) {

    int i;

    // retrieve parameters
    i = *((int *) params);

    // acquire lock
    if (pthread_mutex_lock(&mutex) != SEM_OK)
        DEATH("failed to lock mutex\n")

    printf("thread %d started\n", i);

    while (!finish[i]) {
        // wait on corresponding condition variable
        if (pthread_cond_wait(&cond_var[i], &mutex) != SEM_OK)
            DEATH("failed to wait on cond_var\n")
    }

    printf("thread %d finished\n", i);

    // release lock
    if (pthread_mutex_unlock(&mutex) != SEM_OK)
        DEATH("failed to unlock mutex")

    pthread_exit(EXIT_SUCCESS);
}

int main() {

    pthread_t threads[THREAD_NUM];
    int index[THREAD_NUM], active_threads, killed_thread;

    // initialize mutex, condition variables and finish array
    if (pthread_mutex_init(&mutex, NULL) != SEM_OK)
        DEATH("failed to initialize mutex\n")
    for (int i = 0; i < THREAD_NUM; i++) {
        if (pthread_cond_init(&cond_var[i], NULL) != SEM_OK)
            DEATH("failed to initialize conditional variable\n")
        finish[i] = FALSE;
    }

    // create threads
    for (int i = 0; i < THREAD_NUM; i++) {
        index[i] = i;
        if (pthread_create(&threads[i], NULL, threadFunction, (void *) &index[i]) != PTHREAD_OK)
            DEATH("pthread create failed\n")
    }

    // loop to ask user which thread terminate
    active_threads = THREAD_NUM;
    while (active_threads > 0) {
        // print thread list
        printf("active threads are\n");
        for (int i = 0; i < THREAD_NUM; i++) {
            if (!finish[i]) {
                printf("%d) %lu\n", i, threads[i]);
            }
        }

        // ask user which tread to terminate
        printf("select thread to terminate\n");
        scanf("%d", &killed_thread);

        if (killed_thread < THREAD_NUM) {
            if(!finish[killed_thread]) {
                // acquire lock
                if (pthread_mutex_lock(&mutex) != SEM_OK)
                DEATH("failed to lock mutex\n")

                // signal thread to finish
                finish[killed_thread] = TRUE;
                if (pthread_cond_signal(&cond_var[killed_thread]) != SEM_OK)
                DEATH("failed to signal \n")
                active_threads--;

                // release lock
                if (pthread_mutex_unlock(&mutex) != SEM_OK)
                DEATH("failed to unlock mutex")

                // join finished threads
                pthread_join(threads[killed_thread], NULL);
            }

        }
    }

    // destroy mutex and cond_var
    if (pthread_mutex_destroy(&mutex) != SEM_OK)
        DEATH("failed to destroy mutex\n")
    for (int i = 0; i < THREAD_NUM; i++) {
        if (pthread_cond_destroy(&cond_var[i]) != SEM_OK)
            DEATH("failed to destroy cond_var\n")
    }

    return 0;
}
