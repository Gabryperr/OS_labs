#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
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

    // create the FIFO
    if (mkfifo(FIFO_NAME, S_IRWXU) == -1) DEATH("mkfifo")

    // open the fifo for reading
    if ((fifo_fd = open(FIFO_NAME, O_RDWR)) == -1) {
        if (unlink(FIFO_NAME) == -1) DEATH("unlink")
        DEATH("open")
    }

    // loop to accept messages from client
    while (strcmp(message, TERMINATION) != 0) {
        // read new message from the fifo
        if (read(fifo_fd, message, LENGTH) == -1) {
            if (close(fifo_fd) == -1) DEATH("close")
            if (unlink(FIFO_NAME) == -1) DEATH("unlink")
            DEATH("read")
        }

        // look for house or case
        if (strcmp(message, "house") == 0 || strcmp(message, "casa") == 0) {
            printf("house detected\n");
        }
    }

    // termination received close and exit
    if (close(fifo_fd) == -1) DEATH("close")
    if (unlink(FIFO_NAME) == -1) DEATH("unlink")

    return 0;
}
