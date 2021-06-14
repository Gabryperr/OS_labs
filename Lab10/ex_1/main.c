#include <math.h>
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


pthread_mutex_t m;
double sum;


void *threadFunction(void *params) {

    int i;

    // retrieve parameters
    i = *((int *) params);

    // perform critical operation
    if (pthread_mutex_lock(&m) != SEM_OK) DEATH("failed to lock mutex\n")
    sum += 1.0 / pow(2.0, i);
    if (pthread_mutex_unlock(&m) != SEM_OK) DEATH("failed to unlock mutex\n")

    pthread_exit(EXIT_SUCCESS);
}


int main() {

    int x, *index;
    pthread_t *threads;

    // get x
    printf("insert x value\n");
    scanf("%d", &x);

    // initialize shared sum
    sum = 0;

    // initialize mutex
    if (pthread_mutex_init(&m, NULL) != SEM_OK) DEATH("failed to initialize mutex\n")

    // allocate space for threads identifiers and index
    if ((threads = (pthread_t *) malloc(sizeof(pthread_t) * (x + 1))) == NULL)
        DEATH("failed to allocate memory for threads\n")
    if ((index = (int *) malloc(sizeof(int) * (x + 1))) == NULL)
        DEATH("failed to allocate memory for indexes\n")

    // spawn threads
    for (int i = 0; i < x + 1; i++) {
        index[i] = i;
        if (pthread_create(&threads[i], NULL, threadFunction, (void *) &index[i]) != PTHREAD_OK)
            DEATH("pthread create failed\n")
    }

    // wait for threads to finish
    for (int i = 0; i < x + 1; i++) {
        if (pthread_join(threads[i], NULL) != PTHREAD_OK) DEATH("pthread join failed\n")
    }

    // free memory
    free(threads);
    free(index);

    // destroy mutex
    if(pthread_mutex_destroy(&m) != SEM_OK) DEATH("pthread mutex destroy failed\n")

    // print final sum
    printf("final sum is %lf\n", sum);

    return 0;
}
