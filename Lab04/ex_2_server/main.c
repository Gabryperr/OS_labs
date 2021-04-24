#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/types.h>
#define DEATH(message)      \
    {                       \
        perror(message);    \
        exit(EXIT_FAILURE); \
    }
#define TERMINATION "exit\n"
#define FILE_PATH "/home/gabriele/Scrivania/OS_labs/Lab04/temp.txt"
#define LENGTH 256
#define MESSAGE_TYPE 1

struct msg_struct {
    long msg_type;
    char text[LENGTH];
};

int main() {

    key_t key;
    int msg_id;
    struct msg_struct command;

    // generate the key
    key = ftok(FILE_PATH, 'A');

    // create the queue
    if ((msg_id = msgget(key, IPC_CREAT | S_IRWXU)) == -1)
        DEATH("msgget")

    // start the endless loop to execute the commands
    while (1) {

        // read new command from the queue
        if (msgrcv(msg_id, &command, LENGTH, MESSAGE_TYPE, 0) == -1) {
            // close queue
            if (msgctl(msg_id, IPC_RMID, NULL) == -1) DEATH("msgctl")
            DEATH("msgsnd")
        }

        // check if it is termination
        if (strcmp(command.text, TERMINATION) == 0) {
            // close queue
            if (msgctl(msg_id, IPC_RMID, NULL) == -1) DEATH("msgctl")
            exit(EXIT_SUCCESS);
        } else {
            // execute the command
            system(command.text);
        }
    }

    return 0;
}
