#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#define RANDOM_NUMBERS 100
#define BUF_SIZE 100
#define FILENAME "random.txt"

int main() {

    pid_t pid;
    int fd, status;
    char buf[BUF_SIZE];


    // fork the child to generate random.txt files
    pid = fork();
    switch (pid) {
        case -1:
            printf("error using fork random generator child\n");
            exit(EXIT_FAILURE);
        case 0:
            // open the file
            if ((fd = open(FILENAME, O_WRONLY | O_CREAT | O_TRUNC, 0666)) < 0) {
                perror("Error opening file\n");
                exit(EXIT_FAILURE);
            }
            // write the file
            srandom(time(NULL));
            for (int i = 0; i < RANDOM_NUMBERS; i++) {
                sprintf(buf, "%d ", (int) (random() % 1000));
                write(fd, buf, strlen(buf));
            }
            close(fd);
            exit(EXIT_SUCCESS);
        default:
            wait(&status);
            if (!WIFEXITED(status)) {
                printf("random generator child not exited correctly\n");
                exit(EXIT_FAILURE);
            }
            if (WEXITSTATUS(status) == EXIT_FAILURE) {
                printf("error in random generator child\n");
                exit(EXIT_FAILURE);
            }
    }

    // fork the child to count the number of bytes
    pid = fork();
    switch (pid) {
        case -1:
            printf("error using fork wc -m child\n");
            exit(EXIT_FAILURE);
        case 0:
            execlp("wc", "wc", "-m", FILENAME, NULL);
        default:
            printf("launched wc -m\n");
    }

    // fork the child to count the number of words
    pid = fork();
    switch (pid) {
        case -1:
            printf("error using fork wc -w child\n");
            exit(EXIT_FAILURE);
        case 0:
            execlp("wc", "wc", "-w", FILENAME, NULL);
        default:
            printf("launched wc -w\n");
    }

    // fork the child to compute the mean
    pid = fork();
    switch (pid) {
        case -1:
            printf("error using fork mean child\n");
            exit(EXIT_FAILURE);
        case 0:
            execl("/home/gabriele/Scrivania/OS_labs/Lab03/ex_1_sub/cmake-build-debug/ex_1_sub",
                  "ex_1_sub", FILENAME, NULL);
        default:
            printf("launched mean compute\n");
    }

    // wait for children to finish
    for (int i = 0; i < 3; i++) {
        wait(NULL);
    }

    return 0;
}
