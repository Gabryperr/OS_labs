#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define PRIME 1
#define NOT_PRIME 0
#define DEATH(message)         \
    {                          \
        printf("%s", message); \
        exit(EXIT_FAILURE);    \
    }


// struct for thread prime parameters
struct threadPrimeParams {
    int start_pos;
    int end_pos;
};

struct threadJoinParams {
    int thread_num;
    pthread_t *threads;
};

// shared value to check if the number is prime
int is_prime = PRIME;

// shared value to check
long long number_to_check;

// shared value to compute the elapsed time
clock_t start_time;


// function to calculate the elapsed time
void elapsedTime() {
    printf("elapsed time %lf us", (((double) (clock() - start_time)) / CLOCKS_PER_SEC) * 1000000);
}


// threads that check the prime number
void *threadPrimeFunction(void *params) {

    int start_pos, end_pos;

    //retrieve parameters
    start_pos = ((struct threadPrimeParams *) params)->start_pos;
    end_pos = ((struct threadPrimeParams *) params)->end_pos;

    // check the prime number
    for (int i = start_pos; i < end_pos; i++) {
        if (i > 1 && (number_to_check % i) == 0) {
            is_prime = NOT_PRIME;
        }
    }
    pthread_exit(EXIT_SUCCESS);
}

//thread that check when all threadPrimeFunction have finished
void *threadJoinFunction(void *params) {

    int num_threads;
    pthread_t *threads;

    //retrieve parameters
    num_threads = ((struct threadJoinParams *) params)->thread_num;
    threads = ((struct threadJoinParams *) params)->threads;

    // join all the threads
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    // if all threads have finished, the number is prime
    printf("the number %lld is prime\n", number_to_check);

    // compute elapsed time
    elapsedTime();

    // exit the program
    exit(EXIT_SUCCESS);
}


int main() {

    int thread_num;
    struct threadPrimeParams *params_array;
    struct threadJoinParams join_params;
    pthread_t *threads;
    pthread_t join_thread;


    // get the values from input
    printf("insert the number to check\n");
    scanf("%lld", &number_to_check);
    printf("insert the number of threads\n");
    scanf("%d", &thread_num);

    // allocate vectors for params an thread pids
    if ((params_array = (struct threadPrimeParams *) malloc(sizeof(struct threadPrimeParams) * thread_num)) == NULL) DEATH("malloc failed params array\n")
    if ((threads = (pthread_t *) malloc(sizeof(pthread_t) * thread_num)) == NULL) DEATH("malloc failed threads\n")

    // start to count times
    start_time = clock();

    // launch prime threads
    for (int i = 0; i < thread_num - 1; i++) {
        params_array[i].start_pos = ((number_to_check / 2) / thread_num) * i;
        params_array[i].end_pos = ((number_to_check / 2) / thread_num) * (i + 1);
        if (pthread_create(&threads[i], NULL, threadPrimeFunction, (void *) &params_array[i]) != 0) DEATH("pthread_create failed\n")
    }
    // launch last prime threads
    params_array[thread_num - 1].start_pos = ((number_to_check / 2) / thread_num) * (thread_num - 1);
    params_array[thread_num - 1].end_pos = (number_to_check / 2) + 1;
    if (pthread_create(&threads[thread_num - 1], NULL, threadPrimeFunction, (void *) &params_array[thread_num - 1]) != 0)
        DEATH("pthread_create failed\n")

    // launch join thread
    join_params.threads = threads;
    join_params.thread_num = thread_num;
    if (pthread_create(&join_thread, NULL, threadJoinFunction, (void *) &join_params) != 0) DEATH("pthread_create failed\n")

    // enter loop to check when prime becomes not prime
    while (is_prime == PRIME)
        ;

    // if the while exit the number is not PRIME
    printf("the number %lld is not prime\n", number_to_check);

    // compute elapsed time
    elapsedTime();


    return 0;
}
