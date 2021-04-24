#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#define DEATH(message)      \
    {                       \
        perror(message);    \
        exit(EXIT_FAILURE); \
    }

#define FILE_NAME "number.dat"
#define MEMORY_NAME "/myMemory"

int main() {
    FILE *fp;
    pid_t pid;
    int shm_fd;
    int *shared_num;

    // open the shared memory
    if ((shm_fd = shm_open(MEMORY_NAME, O_CREAT | O_RDWR, S_IRWXU)) == -1)
    DEATH("shm_open")

    // truncate the shared memory
    if (ftruncate(shm_fd, sizeof(int)) == -1)
    DEATH("ftruncate")

    // map the shared memory
    if ((shared_num = (int *) mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0)) == MAP_FAILED)
    DEATH("mmap")

    // initialize the shared memory
    *shared_num = -1;

    // fork thr child process
    pid = fork();
    switch (pid) {
        case -1:
        DEATH("fork")

        case 0:
            // child process,check termination value
            while (*shared_num != 0) {
                // wait for parent to put a new value
                *shared_num = -1;
                while (*shared_num == -1) {}

                // print new value and send confirmation to father
                printf("new number %d\n", *shared_num);
            }

            // received termination value close shared memory and exit
            if (munmap(shared_num, sizeof(int)) == -1) DEATH("munmap child")
            exit(EXIT_SUCCESS);

        default:
            // father process, open the file
            if ((fp = fopen(FILE_NAME, "r")) == NULL) {
                // send the termination value to child, wait for child termination, remove shared memory
                *shared_num = 0;
                wait(NULL);
                if (munmap(shared_num, sizeof(int)) == -1) DEATH("munmap father")
                if (shm_unlink(MEMORY_NAME) == -1) DEATH("shm_unlink father")
                DEATH("fopen")
            }
            // read the file until a 0 is found
            while (*shared_num != 0) {
                // wait for child to process the number
                while (*shared_num != -1) {}
                fscanf(fp, "%d", shared_num);// NOLINT(cert-err34-c)
            }
            fclose(fp);
            wait(NULL);
            if (munmap(shared_num, sizeof(int)) == -1) DEATH("munmap father")
            if (shm_unlink(MEMORY_NAME) == -1) DEATH("shm_unlink father")
    }

    return 0;
}
