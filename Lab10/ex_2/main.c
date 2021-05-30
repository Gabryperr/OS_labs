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

struct threadParams {
    int i;
    double x;
};


pthread_mutex_t m;
double product;


void *threadFunction(void *params) {

    struct threadParams internal_params;

    // retrieve parameters
    internal_params = *((struct threadParams *) params);

    // perform critical operation
    if (pthread_mutex_lock(&m) != SEM_OK) DEATH("failed to lock mutex")
    product *= pow(internal_params.x, internal_params.i);
    if (pthread_mutex_unlock(&m) != SEM_OK) DEATH("failed to unlock mutex")

    pthread_exit(EXIT_SUCCESS);
}


int main() {

    double x;
    int n;
    pthread_t *threads;
    struct threadParams *thread_params;

    // get x and n
    printf("insert x value\n");
    scanf("%lf", &x);
    printf("insert n value\n");
    scanf("%d", &n);

    // initialize shared product
    product = 1.0;

    // initialize mutex
    if (pthread_mutex_init(&m, NULL) != SEM_OK) DEATH("failed to initialize mutex\n")

    // allocate space for threads identifiers and thread params
    if ((threads = (pthread_t *) malloc(sizeof(pthread_t) * (n + 1))) == NULL)
    DEATH("failed to allocate memory for threads\n")
    if ((thread_params = (struct threadParams *) malloc(sizeof(struct threadParams) * (n + 1))) == NULL)
    DEATH("failed to allocate memory for indexes\n")

    // spawn threads
    for (int i = 0; i < n + 1; i++) {
        thread_params[i].i = i;
        thread_params[i].x = x;
        if (pthread_create(&threads[i], NULL, threadFunction, (void *) &thread_params[i]) != PTHREAD_OK)
        DEATH("pthread create failed\n")
    }

    // wait for threads to finish
    for (int i = 0; i < n + 1; i++) {
        if (pthread_join(threads[i], NULL) != PTHREAD_OK) DEATH("pthread join failed\n")
    }

    // free memory
    free(threads);
    free(thread_params);

    // destroy mutex
    if (pthread_mutex_destroy(&m) != SEM_OK) DEATH("pthread mutex destroy failed\n")

    // print final product
    printf("final product is %lf\n", product);

    return 0;
}