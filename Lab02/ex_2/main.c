#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {

    pid_t childPid;

    for (int i = 0; i < 3; i++) {
        childPid = fork();
        switch (childPid) {
            case -1:
                printf("Error using fork()\n");
                exit(1);
            case 0:
                printf("I am the child, my pid is %d, my parent pid is %d\n", getpid(), getppid());
                exit(0);
            default:
                wait(NULL);
        }
    }

    return 0;
}
