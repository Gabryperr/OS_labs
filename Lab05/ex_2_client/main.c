#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define FIFO_NAME "/home/gabriele/Scrivania/OS_labs/Lab05/myFIFO"
#define LENGTH 128
#define TERMINATION "exit"
#define DEATH(message)      \
    {                       \
        perror(message);    \
        exit(EXIT_FAILURE); \
    }

int main() {

    int fifo_fd;
    char message[LENGTH];

    // open the fifo for writing
    if ((fifo_fd = open(FIFO_NAME, O_WRONLY)) == -1) DEATH("open")

    // loop to send message to server
    while (strcmp(message, TERMINATION) != 0) {
        // read from stdin
        printf("enter new message \n");
        scanf("%s", message);

        // write message to the fifo
        if (write(fifo_fd, message, strlen(message) + 1) == -1) {
            if (close(fifo_fd) == -1) DEATH("close")
            DEATH("write")
        }
    }

    // termination received close and exit
    if (close(fifo_fd) == -1) DEATH("close")

    return 0;
}
