#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#define TRUE 1
#define FALSE 0
#define WAIT_FAIL -1
#define FORK_FAIL -1
#define FORK_CHILD 0
#define PSHARED 1
#define SHM_FAIL -1
#define SEM_FAIL -1
#define FILENAME "numbers.dat"
#define PDEATH(message)     \
    {                       \
        perror(message);    \
        exit(EXIT_FAILURE); \
    }
#define DEATH(message)         \
    {                          \
        printf("%s", message); \
        exit(EXIT_FAILURE);    \
    }

int main() {
    pid_t child_even, child_odd;
    sem_t *sem_even, *sem_odd, *sem_father;
    int id_even, id_odd, id_father, id_new_data, id_finish;
    double *new_data;
    int *finish;
    int accumulator, counter, temp_data;
    FILE *fp;

    // create shared memory id
    if ((id_father = shmget(IPC_PRIVATE, sizeof(sem_t), S_IRWXU | IPC_CREAT)) == SHM_FAIL)
        PDEATH("failed to create shared memory for father semaphore\n")
    if ((id_even = shmget(IPC_PRIVATE, sizeof(sem_t), S_IRWXU | IPC_CREAT)) == SHM_FAIL)
        PDEATH("failed to create shared memory for even semaphore\n")
    if ((id_odd = shmget(IPC_PRIVATE, sizeof(sem_t), S_IRWXU | IPC_CREAT)) == SHM_FAIL)
        PDEATH("failed to create shared memory for odd semaphore\n")
    if ((id_new_data = shmget(IPC_PRIVATE, sizeof(int), S_IRWXU | IPC_CREAT)) == SHM_FAIL)
        PDEATH("failed to create shared memory for new data\n")
    if ((id_finish = shmget(IPC_PRIVATE, sizeof(int), S_IRWXU | IPC_CREAT)) == SHM_FAIL)
        PDEATH("failed to create shared memory for finish \n")


    // attach shared memory
    if ((sem_father = (sem_t *) shmat(id_father, NULL, 0)) == (sem_t *) SHM_FAIL)
        PDEATH("failed to attach shared memory for father semaphore\n")
    if ((sem_even = (sem_t *) shmat(id_even, NULL, 0)) == (sem_t *) SHM_FAIL)
        PDEATH("failed to attach shared memory for even semaphore\n")
    if ((sem_odd = (sem_t *) shmat(id_odd, NULL, 0)) == (sem_t *) SHM_FAIL)
        PDEATH("failed to attach shared memory for odd semaphore\n")
    if ((new_data = (double *) shmat(id_new_data, NULL, 0)) == (double *) SHM_FAIL)
        PDEATH("failed to attach shared memory for new data\n")
    if ((finish = (int *) shmat(id_finish, NULL, 0)) == (int *) SHM_FAIL)
        PDEATH("failed to attach shared memory for finish\n")

    // initialize semaphore
    if (sem_init(sem_father, PSHARED, 0) == SEM_FAIL)
        PDEATH("failed to initialize father semaphore\n")
    if (sem_init(sem_even, PSHARED, 0) == SEM_FAIL)
        PDEATH("failed to initialize event semaphore\n")
    if (sem_init(sem_odd, PSHARED, 0) == SEM_FAIL)
        PDEATH("failed to initialize event semaphore\n")

    // initialize accumulator and counter
    accumulator = counter = 0;

    //initialize finish
    *finish = FALSE;

    // create children
    if ((child_even = fork()) == FORK_FAIL)
        PDEATH("failed to fork child even\n")
    if (child_even == FORK_CHILD) {// even child
        while (TRUE) {
            if (sem_wait(sem_even) == SEM_FAIL)
                PDEATH("failed to wait on semaphore even\n")
            // critical section
            if (!*finish) {
                // increment counter and sum new data to accumulator
                accumulator += (int) *new_data;
                counter++;
                printf("even child: accumulator: %d, counter: %d\n", accumulator, counter);
            } else {
                // calculate final mean using new data to share the value with father
                if (counter != 0) {
                    *new_data = (double) accumulator / (double) counter;
                    printf("even child: calculate mean: %lf\n", *new_data);
                }
                // detach shared memory
                if (shmdt(sem_father) == SHM_FAIL)
                    PDEATH("failed to detach shared memory for semaphore father\n")
                if (shmdt(sem_even) == SHM_FAIL)
                    PDEATH("failed to detach shared memory for semaphore even\n")
                if (shmdt(sem_odd) == SHM_FAIL)
                    PDEATH("failed to detach shared memory for semaphore odd\n")
                if (shmdt(new_data) == SHM_FAIL)
                    PDEATH("failed to detach shared memory for new data\n")
                if (shmdt(finish) == SHM_FAIL)
                    PDEATH("failed to detach shared memory for finish\n")

                // computation finished exit
                exit(EXIT_SUCCESS);
            }
            if (sem_post(sem_father) == SEM_FAIL)
                PDEATH("failed to post semaphore father\n")
        }

    } else {
        if ((child_odd = fork()) == FORK_FAIL)
            PDEATH("failed to fork child odd\n")

        if (child_odd == FORK_CHILD) {// odd child
            while (TRUE) {
                if (sem_wait(sem_odd) == SEM_FAIL)
                    PDEATH("failed to wait on semaphore odd\n")
                // critical section
                if (!*finish) {
                    // increment counter and sum new data to accumulator
                    accumulator += (int) *new_data;
                    counter++;
                    printf("odd child: accumulator: %d, counter: %d\n", accumulator, counter);
                } else {
                    // calculate final mean using new data to share the value with father
                    if (counter != 0) {
                        *new_data = (double) accumulator / (double) counter;
                        printf("odd child: calculate mean: %lf\n", *new_data);
                    }
                    // detach shared memory
                    if (shmdt(sem_father) == SHM_FAIL)
                        PDEATH("failed to detach shared memory for semaphore father\n")
                    if (shmdt(sem_even) == SHM_FAIL)
                        PDEATH("failed to detach shared memory for semaphore even\n")
                    if (shmdt(sem_odd) == SHM_FAIL)
                        PDEATH("failed to detach shared memory for semaphore odd\n")
                    if (shmdt(new_data) == SHM_FAIL)
                        PDEATH("failed to detach shared memory for new data\n")
                    if (shmdt(finish) == SHM_FAIL)
                        PDEATH("failed to detach shared memory for finish\n")

                    // computation finished exit
                    exit(EXIT_SUCCESS);
                }
                if (sem_post(sem_father) == SEM_FAIL)
                    PDEATH("failed to post semaphore father\n")
            }

        } else {// father
            // open file
            if ((fp = fopen(FILENAME, "r")) == NULL)
                PDEATH("failed to open file\n")

            // read file
            while (!feof(fp)) {
                // read new number
                fscanf(fp, "%d", &temp_data);

                // critical section pass number to correct child
                *new_data = (double) temp_data;
                if (temp_data % 2 == 0) {// new data is even
                    if (sem_post(sem_even) == SEM_FAIL)
                        PDEATH("failed to post semaphore even\n")
                } else {// new data is odd
                    if (sem_post(sem_odd) == SEM_FAIL)
                        PDEATH("failed to post semaphore odd\n")
                }
                if (sem_wait(sem_father) == SEM_FAIL)
                    PDEATH("failed to wait on semaphore father\n")
            }
            // finished the file, close it
            if (fclose(fp) == EOF)
                PDEATH("failed to close file\n")
            // terminate computation for children
            *finish = TRUE;

            // print even mean
            if (sem_post(sem_even) == SEM_FAIL)
                PDEATH("failed to post semaphore even\n")
            if (waitpid(child_even, NULL, 0) == WAIT_FAIL)
                DEATH("failed to wait even child completion\n")
            printf("even mean is %lf\n", *new_data);


            // print odd mean
            if (sem_post(sem_odd) == SEM_FAIL)
                PDEATH("failed to post semaphore odd\n")
            if (waitpid(child_odd, NULL, 0) == WAIT_FAIL)
                DEATH("failed to wait odd child completion\n")
            printf("odd mean is %lf\n", *new_data);


            // destroy semaphore
            if (sem_destroy(sem_father) == SEM_FAIL)
                PDEATH("failed to destroy semaphore father\n")
            if (sem_destroy(sem_even) == SEM_FAIL)
                PDEATH("failed to destroy semaphore even\n")
            if (sem_destroy(sem_odd) == SEM_FAIL)
                PDEATH("failed to destroy semaphore odd\n")

            // detach shared memory
            if (shmdt(sem_father) == SHM_FAIL)
                PDEATH("failed to detach shared memory for semaphore father\n")
            if (shmdt(sem_even) == SHM_FAIL)
                PDEATH("failed to detach shared memory for semaphore even\n")
            if (shmdt(sem_odd) == SHM_FAIL)
                PDEATH("failed to detach shared memory for semaphore odd\n")
            if (shmdt(new_data) == SHM_FAIL)
                PDEATH("failed to detach shared memory for new data\n")
            if (shmdt(finish) == SHM_FAIL)
                PDEATH("failed to detach shared memory for finish\n")

            //destroy shared memory
            if (shmctl(id_father, IPC_RMID, NULL) == SHM_FAIL)
                PDEATH("failed to destroy shared memory for semaphore father\n")
            if (shmctl(id_even, IPC_RMID, NULL) == SHM_FAIL)
                PDEATH("failed to destroy shared memory for semaphore even\n")
            if (shmctl(id_odd, IPC_RMID, NULL) == SHM_FAIL)
                PDEATH("failed to destroy shared memory for semaphore odd\n")
            if (shmctl(id_new_data, IPC_RMID, NULL) == SHM_FAIL)
                PDEATH("failed to destroy shared memory for new data\n")
            if (shmctl(id_finish, IPC_RMID, NULL) == SHM_FAIL)
                PDEATH("failed to destroy shared memory for finish\n")
        }
    }

    return 0;
}
