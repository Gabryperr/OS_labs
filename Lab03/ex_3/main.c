#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {

    pid_t pid;
    int shm_id, max_n, status;
    double base, sum;
    double *partial_terms;

    if (argc != 3) {
        printf("Usage: provide base and N");
        exit(EXIT_FAILURE);
    }

    // parse base and N
    base = atof(argv[1]); // NOLINT(cert-err34-c)
    max_n = atoi(argv[2]);// NOLINT(cert-err34-c)

    // create the shared memory
    if ((shm_id = shmget(IPC_PRIVATE, (max_n + 1) * sizeof(double), IPC_CREAT | S_IRWXU)) == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    // attach the shared memory
    if ((partial_terms = (double *) shmat(shm_id, NULL, 0)) == (double *) -1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    // fork child process
    for (int i = 0; i <= max_n; i++) {
        pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);

        } else if (pid == 0) {
            // child process, compute the partial_terms
            partial_terms[i] = pow(base, (double) i);
            // detach shared memory from child
            if (shmdt(partial_terms) == -1){
                perror("shmdt");
                exit(EXIT_FAILURE);
            }
            exit(EXIT_SUCCESS);
        }
    }

    // wait for child process to finish
    for (int i = 0; i <= max_n; i++) {
        wait(&status);
        if (!WIFEXITED(status)) {
            printf("child non terminated correctly\n");
            exit(EXIT_FAILURE);
        }
    }

    // compute the sum
    sum = 0.0;
    for (int i = 0; i <= max_n; i++) {
        sum += partial_terms[i];
    }

    // display the result
    printf("the result is %lf\n", sum);

    // detach shared memory
    if (shmdt(partial_terms) == -1){
        perror("shmdt");
        exit(EXIT_FAILURE);
    }

    // delete shared memory
    if (shmctl(shm_id, IPC_RMID, NULL) == -1){
        perror("shmctl");
        exit(EXIT_FAILURE);
    }

    return 0;
}
