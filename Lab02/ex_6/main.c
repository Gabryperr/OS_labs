#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char **argv) {

    int finalSum = 0;
    int partialValue = 1;
    int n;
    pid_t childPid;

    if (argc != 2) {
        printf("Usage: pass one value between 0 and 5");
        exit(1);
    }
    n = (int) strtol(argv[1], NULL, 10);

    for (int i = 0; i <= n; i++) {
        childPid = fork();
        switch (childPid) {
            case -1:
                printf("Error using fork()");
                exit(1);
            case 0:
                for (int j = 0; j < i; j++) {
                    partialValue *= 2;
                }
                printf("%d\n", partialValue);
                exit(partialValue);
            default:
                break;
        }
    }

    for (int i = 0; i <= n; i++) {
        wait(&partialValue);
        if (WIFEXITED(partialValue)) {
            finalSum += WEXITSTATUS(partialValue);
        } else {
            printf("child exited abnormally\n");
        }
    }

    printf("final sum is %d\n", finalSum);

    return 0;
}
