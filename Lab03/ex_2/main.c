#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define COMMAND_LENGTH 200

int main(int argc, char *argv[]) {

    char command[COMMAND_LENGTH];

    if (argc != 2) {
        printf("Usage: provide the find arguments\n");
        exit(EXIT_FAILURE);
    }
    printf("Launching find\n");
    strcpy(command, "find ");
    strcat(command, argv[1]);

    system(command);

    return 0;
}
