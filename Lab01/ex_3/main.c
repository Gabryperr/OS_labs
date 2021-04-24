#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {

    long number;

    if (argc != 2) {
        printf("Usage: input only one integer number\n");
        exit(1);
    }

    number = strtol(argv[1], NULL, 10);

    printf(number % 2 == 0 ? "even number" : "odd number");

    return 0;
}
