#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#define READ 0
#define WRITE 1
#define LENGTH 128
#define DEATH(message)      \
    {                       \
        perror(message);    \
        exit(EXIT_FAILURE); \
    }

int main(int argc, char *argv[]) {

    pid_t pid;
    int max_n, status, pipe_fd[2];
    double partial_value, final_sum;
    char partial_value_str[LENGTH];

    if (argc != 2) {
        printf("Usage: provide N\n");
        exit(EXIT_FAILURE);
    }

    // read max_n
    sscanf(argv[1], "%d", &max_n);// NOLINT(cert-err34-c)

    // create pipe
    if (pipe(pipe_fd) == -1)
        DEATH("pipe create failed")

    // initialize final_sum
    final_sum = 0.0;

    // fork the children process
    for (int i = 1; i < max_n; i++) {
        pid = fork();
        switch (pid) {
            case -1:
                // error close pipe and exit
                close(pipe_fd[READ]);
                close(pipe_fd[WRITE]);
                DEATH("fork")

            case 0:
                // compute the value
                partial_value = exp((double) i) / (double) i;

                // convert to string
                sprintf(partial_value_str, "%lf", partial_value);

                // send to pipe
                if (write(pipe_fd[WRITE], partial_value_str, strlen(partial_value_str)) == -1) DEATH("write failed")
                exit(EXIT_SUCCESS);

            default:
                // wait for child to finish
                wait(&status);
                if(!WIFEXITED(status)){
                    printf("child not terminated correctly\n");
                    close(pipe_fd[READ]);
                    close(pipe_fd[WRITE]);
                    exit(EXIT_FAILURE);
                }

                // read new value from the pipe
                if (read(pipe_fd[READ], partial_value_str, LENGTH) == -1){
                    close(pipe_fd[READ]);
                    close(pipe_fd[WRITE]);
                    DEATH("read failed")
                }

                // sum the new value received
                sscanf(partial_value_str, "%lf", &partial_value); // NOLINT(cert-err34-c)
                final_sum += partial_value;
        }
    }

    // display final value
    printf("final sum is %lf\n", final_sum);

    //close pipe
    close(pipe_fd[READ]);
    close(pipe_fd[WRITE]);

    return 0;
}
