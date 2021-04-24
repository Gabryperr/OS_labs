#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#define FILE_PATH "/home/gabriele/Scrivania/OS_labs/Lab03/temp.txt"
#define LENGTH 256
#define TERMINATION "exit"

int main() {
    key_t key_command, key_flag;
    int shm_command_id, shm_flag_id;
    int *flag;
    char *command;

    // generate the key
    key_command = ftok(FILE_PATH, 'A');
    key_flag = ftok(FILE_PATH, 'B');

    // create shared memory
    if ((shm_command_id = shmget(key_command, LENGTH * sizeof(char), IPC_CREAT | S_IRWXU)) == -1) {
        perror("shmget command");
        exit(EXIT_FAILURE);
    }
    if ((shm_flag_id = shmget(key_flag, sizeof(int), IPC_CREAT | S_IRWXU)) == -1) {
        perror("shmget flag");
        exit(EXIT_FAILURE);
    }

    // attach the shared memory
    if ((command = (char *) shmat(shm_command_id, NULL, 0)) == (char *) -1) {
        perror("shmat command");
        exit(EXIT_FAILURE);
    }
    if ((flag = (int *) shmat(shm_flag_id, NULL, 0)) == (int *) -1) {
        perror("shmat flag");
        exit(EXIT_FAILURE);
    }

    // start the endless loop to accept the commands
    while (1) {

        // wait for the server to clear the flag
        while (*flag != 0) {}

        // read new command
        printf("waiting for command ");
        scanf("%s", command);

        // set the flag
        *flag = 1;

        // check if it is termination
        if (strcmp(command, TERMINATION) == 0) {
            // detach memory
            if (shmdt(command) == -1) {
                perror("shmdt command");
                exit(EXIT_FAILURE);
            }
            if (shmdt(flag) == -1) {
                perror("shmdt flag");
                exit(EXIT_FAILURE);
            }

            // exit
            exit(EXIT_SUCCESS);
        }
    }

    return 0;
}
