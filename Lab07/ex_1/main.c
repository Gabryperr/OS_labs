#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_LENGTH 50000
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

// strings for sharing sequence and genome
char genome[MAX_LENGTH];
char sequence[MAX_LENGTH];

// struct for thread parameters
struct threadParams {
    int start_pos;
    int end_pos;
};

// function to check if base string is inside target string in a specific position
// return 0 if not match, 1 otherwise
int searchString(char *base_str, const char *target_str, int pos) {

    for (int i = 0; i < strlen(base_str); i++) {
        if (base_str[i] != target_str[pos + i])
            return 0;
    }
    return 1;
}

// thread function
void *threadFunction(void *params) {

    int start_pos;
    int end_pos;

    //retrieve parameters
    start_pos = ((struct threadParams *) params)->start_pos;
    end_pos = ((struct threadParams *) params)->end_pos;

    // search the string
    for (int i = start_pos; i < end_pos; i++) {
        if (searchString(sequence, genome, i) == 1) {
            printf("%d-%d\n", i, i + (int) strlen(sequence) - 1);
        }
    }
    pthread_exit(EXIT_SUCCESS);
}


int main() {

    FILE *fp_sequence, *fp_genome;
    int thread_num;
    struct threadParams *params_array;
    pthread_t *threads;


    // read the sequence file
    if ((fp_sequence = fopen("sequence.txt", "r")) == NULL) PDEATH("sequence file")
    fscanf(fp_sequence, "%s", sequence);
    fclose(fp_sequence);

    // read the genome file
    if ((fp_genome = fopen("genome.txt", "r")) == NULL) PDEATH("genome file")
    fscanf(fp_genome, "%s", genome);
    fclose(fp_genome);

    // ask the user how many thread
    printf("insert number of thread\n");
    scanf("%d", &thread_num);

    // allocate vectors for params an thread pids
    if ((params_array = (struct threadParams *) malloc(sizeof(struct threadParams) * thread_num)) == NULL) DEATH("malloc failed params array\n")
    if ((threads = (pthread_t *) malloc(sizeof(pthread_t) * thread_num)) == NULL) DEATH("malloc failed threads\n")

    // launch threads
    for (int i = 0; i < thread_num - 1; i++) {
        params_array[i].start_pos = ((strlen(genome) - strlen(sequence)) / thread_num) * i;
        params_array[i].end_pos = ((strlen(genome) - strlen(sequence)) / thread_num) * (i + 1);
        if (pthread_create(&threads[i], NULL, threadFunction, (void *) &params_array[i]) != 0) DEATH("pthread_create failed\n")
    }
    // launch last thread
    params_array[thread_num - 1].start_pos = ((strlen(genome) - strlen(sequence)) / thread_num) * (thread_num - 1);
    params_array[thread_num - 1].end_pos = (strlen(genome) - strlen(sequence));
    if (pthread_create(&threads[thread_num - 1], NULL, threadFunction, (void *) &params_array[thread_num - 1]) != 0) DEATH("pthread_create failed\n")

    // wait for threads to finish
    for (int i = 0; i < thread_num; i++){
        if(pthread_join(threads[i], NULL) != 0) DEATH("pthread_join failed\n")
    }

    // free memory
    free(params_array);
    free(threads);

    return 0;
}
