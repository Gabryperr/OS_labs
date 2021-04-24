#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

# define N 10

int main() {
    pid_t pid = 1;

    for (int i = 0; i < N && pid != 0; i++) {

        pid = fork();
        if (pid < 0) {
            printf("Error at fork in iteration %d\n", i);
        } else if (pid == 0) {
            printf("I am child with pid %d\n", getpid());
        }
    }

    return 0;
}
