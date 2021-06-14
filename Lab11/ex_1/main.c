#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define PRIME 1
#define NOT_PRIME 0
#define PTHREAD_OK 0
#define SEM_OK 0
#define DEATH(message)         \
    {                          \
        printf("%s", message); \
        exit(EXIT_FAILURE);    \
    }

struct threadParams {
    int start;
    int stop;
};

// number to check
int n;

// shared number of threads that have finished checking
int finished_threads = 0;

// shared value to check if the number is prime
int is_prime = PRIME;

pthread_mutex_t mutex;
pthread_cond_t cond_var;


void *threadFunction(void *params) {
    int start, stop;

    //retrieve parameters
    start = ((struct threadParams *) params)->start;
    stop = ((struct threadParams *) params)->stop;

    // check the prime number
    for (int i = start; i < stop; i++) {
        if (i > 1 && (n % i) == 0) {
            // acquire lock
            if (pthread_mutex_lock(&mutex) != SEM_OK)
                DEATH("failed to lock mutex\n")

            is_prime = NOT_PRIME;

            // signal to main program that the number is not is_prime
            if (pthread_cond_signal(&cond_var) != SEM_OK)
                DEATH("failed to signal cond_var\n")

            // release lock
            if (pthread_mutex_unlock(&mutex) != SEM_OK)
                DEATH("failed to unlock mutex")
        }
    }
    // acquire lock
    if (pthread_mutex_lock(&mutex) != SEM_OK)
        DEATH("failed to lock mutex\n")

    finished_threads++;

    // signal to main program that one thread has finished
    if (pthread_cond_signal(&cond_var) != SEM_OK)
        DEATH("failed to signal cond_var\n")

    // release lock
    if (pthread_mutex_unlock(&mutex) != SEM_OK)
        DEATH("failed to unlock mutex")

    pthread_exit(EXIT_SUCCESS);
}


int main() {
    int p;
    pthread_t *threads;
    struct threadParams *params;

    // read parameters
    printf("insert number to test\n");
    scanf("%d", &n);
    printf("insert number of threads\n");
    scanf("%d", &p);

    // allocate space for threads identifiers and params
    if ((threads = (pthread_t *) malloc(sizeof(pthread_t) * p)) == NULL)
        DEATH("failed to allocate memory for threads\n")
    if ((params = (struct threadParams *) malloc(sizeof(struct threadParams) * p)) == NULL)
        DEATH("failed to allocate memory for params\n")

    // initialize mutex and condition variable
    if (pthread_mutex_init(&mutex, NULL) != SEM_OK)
        DEATH("failed to initialize mutex\n")
    if (pthread_cond_init(&cond_var, NULL) != SEM_OK)
        DEATH("failed to initialize conditional variable\n")

    // launch prime threads
    for (int i = 0; i < p - 1; i++) {
        params[i].start = ((n / 2) / p) * i;
        params[i].stop = ((n / 2) / p) * (i + 1);
        if (pthread_create(&threads[i], NULL, threadFunction, (void *) &params[i]) != PTHREAD_OK)
            DEATH("pthread_create failed\n")
    }
    // launch last prime threads
    params[p - 1].start = ((n / 2) / p) * (p - 1);
    params[p - 1].stop = (n / 2) + 1;
    if (pthread_create(&threads[p - 1], NULL, threadFunction, (void *) &params[p - 1]) != PTHREAD_OK)
        DEATH("pthread_create failed\n")

    // acquire lock
    if (pthread_mutex_lock(&mutex) != SEM_OK)
        DEATH("failed to lock mutex\n")
    // check if the number is still prime and not all the treads have finished
    while (is_prime && finished_threads != p) {
        if (pthread_cond_wait(&cond_var, &mutex) != SEM_OK)
            DEATH("failed to wait on cond_var\n")
    }
    // release lock
    if (pthread_mutex_unlock(&mutex) != SEM_OK)
        DEATH("failed to unlock mutex")

    if (is_prime) {
        printf("the number %d is prime\n", n);
    } else {
        printf("the number %d is not prime\n", n);
    }

    // destroy mutex and cond_var
    if(pthread_mutex_destroy(&mutex) != SEM_OK)
        DEATH("failed to destroy mutex\n")
    if(pthread_cond_destroy(&cond_var) != SEM_OK)
        DEATH("failed to destroy cond_var\n")

    // free memory
    free(threads);
    free(params);

    return 0;
}
