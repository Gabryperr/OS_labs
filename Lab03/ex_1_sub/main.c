#include <stdio.h>
#include <stdlib.h>
#define RANDOM_NUMBERS 100

int main(int argc, char *argv[]) {
    FILE *fp;
    int accumulator = 0;
    int current;

    if (argc != 2) {
        printf("provide the filename\n");
        exit(EXIT_FAILURE);
    }
    fp = fopen(argv[1], "r");
    for (int i = 0; i < RANDOM_NUMBERS; i++) {
        fscanf(fp, "%d ", &current); // NOLINT(cert-err34-c)
        accumulator += current;
    }
    fclose(fp);
    printf("The mean is %f\n", (float)accumulator/(float)RANDOM_NUMBERS);
}
