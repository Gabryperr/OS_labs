#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define SLEEP_TIME 5
#define DEATH(message)         \
    {                          \
        printf("%s", message); \
        exit(EXIT_FAILURE);    \
    }

void *threadFunction(void *thread_num) {
    printf("%s thread processing\n", (char *) thread_num);
    sleep(SLEEP_TIME);
    printf("%s thread terminates\n", (char *) thread_num);
    pthread_exit(EXIT_SUCCESS);
}

int main() {
    pthread_t thread_ids[2];

    if (pthread_create(&thread_ids[0], NULL, threadFunction, "first") != 0)
        DEATH("error creating first thread\n")
    else
        printf("first thread created\n");

    if (pthread_create(&thread_ids[0], NULL, threadFunction, "second") != 0)
        DEATH("error creating second thread\n")
    else
        printf("second thread created\n");

    pthread_exit(EXIT_SUCCESS);
}
