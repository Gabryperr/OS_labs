#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t childPid;

    for (int i = 0; i < 3; i++) {
        childPid = fork();
        switch (childPid) {
            case -1:
                printf("Error using fork()\n");
                exit(1);
            case 0:
                printf("My pid is %d, my parent is %d\n", getpid(), getppid());
                break;
            default:
                wait(NULL);
                exit(0);
        }
    }

    return 0;
}
