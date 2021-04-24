#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunknown-pragmas"
#pragma ide diagnostic ignored "EndlessLoop"

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define RANDOM_N_CHILDREN 3
#define RANDOM_SQUARE_CHILDREN 3
#define N_RANDOM 50
#define SLEEP_TIME 5
#define MAX_RAND 100

int main() {

    pid_t randomPid[RANDOM_N_CHILDREN];
    pid_t squaredPid[RANDOM_SQUARE_CHILDREN];
    pid_t endedPid;
    int toSquare;

    for (int i = 0; i < RANDOM_N_CHILDREN; i++) {
        randomPid[i] = fork();
        switch (randomPid[i]) {
            case -1:
                printf("Error in fork with RANDOM_N_CHILDREN");
                exit(1);
            case 0:
                srand(time(NULL));// NOLINT(cert-msc51-cpp)
                while (1) {
                    printf("my pid is %d, random number %d\n",
                           getpid(), rand() % MAX_RAND);// NOLINT(cert-msc50-cpp)
                    sleep(SLEEP_TIME);
                }
            default:
                break;
        }
    }

    for (int i = 0; i < RANDOM_SQUARE_CHILDREN; i++) {
        squaredPid[i] = fork();
        switch (squaredPid[i]) {
            case -1:
                printf("Error in fork with RANDOM_N_CHILDREN");
                exit(1);
            case 0:
                srand(time(NULL));// NOLINT(cert-msc51-cpp)
                for (int j = 0; j < N_RANDOM; j++) {
                    toSquare = rand() % MAX_RAND;// NOLINT(cert-msc50-cpp)
                    printf("my pid is %d, random number %d, its square %d\n",
                           getpid(), toSquare, toSquare * toSquare);
                    sleep(SLEEP_TIME);
                }
                exit(0);
            default:
                break;
        }
    }


    while (1) {
        endedPid = wait(NULL);
        for (int i = 0; i < RANDOM_N_CHILDREN; i++) {
            if (randomPid[i] == endedPid) {
                randomPid[i] = fork();
                switch (randomPid[i]) {
                    case -1:
                        printf("Error in fork with RANDOM_N_CHILDREN");
                        exit(1);
                    case 0:
                        srand(time(NULL));// NOLINT(cert-msc51-cpp)
                        while (1) {
                            printf("my squared_pid is %d, random number %d\n",
                                   getpid(), rand() % MAX_RAND);// NOLINT(cert-msc50-cpp)
                            sleep(SLEEP_TIME);
                        }
                    default:
                        break;
                }
            }
        }
        for (int i = 0; i < RANDOM_SQUARE_CHILDREN; i++) {
            if (squaredPid[i] == endedPid) {
                squaredPid[i] = fork();
                switch (squaredPid[i]) {
                    case -1:
                        printf("Error in fork RANDOM_SQUARE_CHILDREN");
                        exit(1);
                    case 0:
                        srand(time(NULL)); // NOLINT(cert-msc51-cpp)
                        for (int j = 0; j < N_RANDOM; j++) {
                            toSquare = rand() % MAX_RAND;// NOLINT(cert-msc50-cpp)
                            printf("my pid is %d, random number %d, its square %d\n",
                                   getpid(), toSquare, toSquare * toSquare);
                            sleep(SLEEP_TIME);
                        }
                        exit(0);
                    default:
                        break;
                }
            }
        }
    }
}
#pragma clang diagnostic pop