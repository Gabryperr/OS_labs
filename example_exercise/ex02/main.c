#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {

    pid_t pid, child_pid;
    FILE *fp;

    pid = fork();
    switch (pid) {
        case -1:
            printf("fork failed");
            exit(-1);

        case 0:
            fp = fopen("abc", "w");
            if(fp == NULL){
                printf("Error opening file");
            }
            else{
                fclose(fp);
            }
            exit(0);

        default:
            child_pid = wait(NULL);
            if(child_pid == -1){
                printf("Error waiting for child process");
            }
            else{
                printf("child terminated with pid %d\n", pid);
            }

    }

    return 0;
}
