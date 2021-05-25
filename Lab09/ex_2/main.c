#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SEM_OK 0
#define PTHREAD_OK 0
#define FILE_OK 0
#define MAX_LENGTH 100
#define N_TIMES 100
#define INCREMENT 3
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

sem_t semaphore;
char filename[MAX_LENGTH];

void *threadFunction() {

    FILE *fp;
    int last_num;

    for (int i = 0; i < N_TIMES; i++) {
        // wait for semaphore
        if (sem_wait(&semaphore) != SEM_OK) PDEATH("failed to wait for semaphore\n")

        // read until last value
        if ((fp = fopen(filename, "r")) == NULL) DEATH("failed to open file in thread function\n")
        while (!feof(fp)) {
            fscanf(fp, "%d", &last_num);// NOLINT(cert-err34-c)
        }
        fclose(fp);

        // write new value
        if ((fp = fopen(filename, "a")) == NULL) DEATH("failed to open file in thread function\n")
        fprintf(fp, "%d\n", last_num + INCREMENT);
        fclose(fp);

        // signal to semaphore
        if (sem_post(&semaphore) != SEM_OK) PDEATH("failed to signal to semaphore\n")
    }
    pthread_exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {

    int num_threads;
    pthread_t *threads;
    FILE *fp;

    if (argc < 3) DEATH("usage: provide filename and number of threads\n")

    // retrieve arguments from command line
    strcpy(filename, argv[1]);
    num_threads = atoi(argv[2]);// NOLINT(cert-err34-c)

    // create file and write first number
    if ((fp = fopen(filename, "w")) == NULL) DEATH("failed to open file in main function\n")
    fprintf(fp, "%d\n", 0);
    if (fclose(fp) != FILE_OK) PDEATH("impossible to close file\n")

    // allocate space for threads identifiers
    if ((threads = (pthread_t *) malloc(sizeof(pthread_t) * num_threads)) == NULL) DEATH("failed to allocate memory for threads\n")

    // initialize semaphore
    if (sem_init(&semaphore, 0, 1) != SEM_OK) PDEATH("failed to initialize semaphore\n")

    // start threads
    for (int i = 0; i < num_threads; i++) {
        if (pthread_create(&threads[i], NULL, threadFunction, NULL) != PTHREAD_OK) DEATH("pthread create failed\n")
    }

    // wait for threads to finish
    for (int i = 0; i < num_threads; i++) {
        if (pthread_join(threads[i], NULL) != PTHREAD_OK) DEATH("pthread join failed\n")
    }

    // free memory
    free(threads);

    // destroy semaphore
    if (sem_destroy(&semaphore) != SEM_OK) PDEATH("failed to destroy semaphore\n")

    return 0;
}
