#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {

    pid_t childPid;

    childPid = fork();

    switch (childPid) {
        case -1:
            printf("error using fork\n");
            exit(1);
        case 0:
            printf("I am the child with pid %d\n", getpid());
            exit(0);
        default:
            printf("I am the parent, my pid is %d, my child pid is %d\n", getpid(), childPid);
    }

    return 0;
}
