#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define STRING_LENGTH 20
#define DEATH(message)         \
    {                          \
        printf("%s", message); \
        exit(EXIT_FAILURE);    \
    }

char shared_string[STRING_LENGTH];

void *threadFunction() {
    while (strcmp(shared_string, "exit") != 0) {
        if (strcmp(shared_string, "house") == 0 || strcmp(shared_string, "casa") == 0) {
            printf("house detected\n");
            strcpy(shared_string, "");
            sleep(1);
        }
    }
    exit(EXIT_SUCCESS);
}


int main() {

    pthread_t thread_id;

    if (pthread_create(&thread_id, NULL, threadFunction, NULL) != 0)
        DEATH("error creating thread")

    while(1){
        printf("insert a message\n");
        scanf("%s", shared_string);
    }
}
