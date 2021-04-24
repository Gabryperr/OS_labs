#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#define CHILDREN 2

int main() {

    pid_t pid[CHILDREN];

    for (int i = 0; i < CHILDREN; i++) {
        pid[i] = fork();
        switch (pid[i]) {
            case -1:
                printf("Error using fork()");
                exit(1);
            case 0:
                sleep(5);
                printf("my pid is %d, my parent is %d\n", getpid(), getppid());
                exit(0);
        }
    }

    waitpid(pid[CHILDREN - 1], NULL, 0);

    return 0;
}
