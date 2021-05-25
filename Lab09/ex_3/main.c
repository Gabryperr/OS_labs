#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#define FORK_FAIL -1
#define FORK_CHILD 0
#define SHM_FAIL -1
#define PSHARED 1
#define SEM_OK 0
#define TRUE 1
#define PDEATH(message)     \
    {                       \
        perror(message);    \
        exit(EXIT_FAILURE); \
    }

int main() {

    pid_t pid;
    int *sv, sv_id, sem_sv_id, r;
    sem_t *sem_sv;


    // get shared memories id
    if ((sv_id = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | S_IRWXU)) == SHM_FAIL)
        PDEATH("failed to get shared memory id\n")
    if ((sem_sv_id = shmget(IPC_PRIVATE, sizeof(sem_t), IPC_CREAT | S_IRWXU)) == SHM_FAIL)
        PDEATH("failed to get shared memory id\n")

    // attach shared memory
    if ((sv = (int *) shmat(sv_id, NULL, 0)) == (void *) SHM_FAIL)
        PDEATH("failed to attach shared memory\n")
    if ((sem_sv = (sem_t *) shmat(sem_sv_id, NULL, 0)) == (void *) SHM_FAIL)
        PDEATH("failed to attach shared memory\n")

    // initialize SV and semaphore
    *sv = -1;
    if (sem_init(sem_sv, PSHARED, 1) != SEM_OK) PDEATH("failed to initialize semaphore\n")

    // initialize randomness
    srandom(time(NULL));

    // create P1
    if ((pid = fork()) == FORK_FAIL) PDEATH("fork failed\n")
    if (pid != FORK_CHILD) {// parent process P
        while (TRUE) {
            r = (int) (random() % 10 + 1);
            sleep(r);

            if (sem_wait(sem_sv) != SEM_OK) PDEATH("failed to wait for semaphore\n")
            *sv = r;
            if (sem_post(sem_sv) != SEM_OK) PDEATH("failed to signal semaphore\n")
        }

    } else {// child process P1
        while (TRUE) {
            r = (int) (random() % 3 + 1);
            sleep(r);
            if ((pid = fork()) == FORK_FAIL) PDEATH("fork failed\n")

            if (pid != FORK_CHILD) {// parent process P1
                if (wait(NULL) == FORK_FAIL) PDEATH("failed to wait for child\n")

            } else {// child process of P1
                r = (int) (random() % 5 + 1);
                sleep(r);

                if (sem_wait(sem_sv) != SEM_OK) PDEATH("failed to wait for semaphore\n")
                if (*sv != -1) {
                    printf("SV equal to %d\n", *sv);
                    *sv = -1;
                }
                if (sem_post(sem_sv) != SEM_OK) PDEATH("failed to signal semaphore\n")

                // detach shared memory
                if (shmdt(sv) == SHM_FAIL) PDEATH("failed to detach shared memory\n")
                if (shmdt(sem_sv) == SHM_FAIL) PDEATH("failed to detach shared memory\n")

                // exit process
                exit(EXIT_SUCCESS);
            }
        }
    }

    return 0;
}
