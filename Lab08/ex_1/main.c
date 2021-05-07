#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define N 31
#define DEATH(message)         \
    {                          \
        printf("%s", message); \
        exit(EXIT_FAILURE);    \
    }


// struct for thread parameters
struct threadParams {
    int *array;
    int start_pos;
    int end_pos;
};


// thread function
void *threadFunction(void *params) {

    struct threadParams center_leaf, left_leaf, right_leaf;
    pthread_t left_thread, right_thread;
    int temp, left_index, right_index, support_index, *support_array;


    // retrieve data from parameters and store in center_leaf
    center_leaf = *((struct threadParams *) (params));

    switch (center_leaf.end_pos - center_leaf.start_pos) {

        case 2:
            // this is the case where two elements are passed, we need to sort them
            if (center_leaf.array[center_leaf.start_pos] > center_leaf.array[center_leaf.end_pos - 1]) {
                // swap elements
                temp = center_leaf.array[center_leaf.start_pos];
                center_leaf.array[center_leaf.start_pos] = center_leaf.array[center_leaf.end_pos - 1];
                center_leaf.array[center_leaf.end_pos - 1] = temp;
            }
            break;
        case 1:
            // this is the case where just one element is in the leaf, nothing to do
            break;
        default:
            // default case we have to create two new threads, left and right

            // parameters calculation
            left_leaf.array = right_leaf.array = center_leaf.array;
            left_leaf.start_pos = center_leaf.start_pos;
            left_leaf.end_pos = right_leaf.start_pos = (center_leaf.start_pos + center_leaf.end_pos) / 2;
            right_leaf.end_pos = center_leaf.end_pos;

            // thread creation
            if (pthread_create(&left_thread, NULL, threadFunction, (void *) &left_leaf) != 0)
                DEATH("pthread_create failed\n")
            if (pthread_create(&right_thread, NULL, threadFunction, (void *) &right_leaf) != 0)
                DEATH("pthread_create failed\n")

            // wait for thread completion
            if (pthread_join(left_thread, NULL) != 0) DEATH("pthread_join failed\n")
            if (pthread_join(right_thread, NULL) != 0) DEATH("pthread_join failed\n")

            // merge the two sub sequence
            left_index = left_leaf.start_pos;
            right_index = right_leaf.start_pos;
            support_index = 0;
            support_array = (int *) malloc(sizeof(int) * (center_leaf.end_pos - center_leaf.start_pos));

            // first merge up to when both of the two have elements
            while (left_index < left_leaf.end_pos && right_index < right_leaf.end_pos) {
                if (center_leaf.array[left_index] < center_leaf.array[right_index]) {
                    support_array[support_index] = center_leaf.array[left_index];
                    left_index++;
                } else {
                    support_array[support_index] = center_leaf.array[right_index];
                    right_index++;
                }
                support_index++;
            }
            // merge the residual elements
            while (left_index < left_leaf.end_pos) {
                support_array[support_index] = center_leaf.array[left_index];
                left_index++;
                support_index++;
            }
            while (right_index < right_leaf.end_pos) {
                support_array[support_index] = center_leaf.array[right_index];
                right_index++;
                support_index++;
            }

            // recopy merged data back to the original array
            for (int i = 0; i < support_index; i++) {
                center_leaf.array[i + center_leaf.start_pos] = support_array[i];
            }

            free(support_array);
    }
    pthread_exit(EXIT_SUCCESS);
}


int main() {

    int random_array[N];
    struct threadParams start_leaf;
    pthread_t start_thread;


    // initialize randomness
    srandom(time(NULL));

    // create the unsorted array
    for (int i = 0; i < N; i++) {
        random_array[i] = (int) (random() >> 16);
    }

    // print the unsorted array
    printf("the unsorted array is:\n");
    for (int i = 0; i < N; i++) {
        printf("%d ", random_array[i]);
    }

    printf("\n\nsorting...:\n\n");

    // sort the array by creating the first thread
    start_leaf.array = random_array;
    start_leaf.start_pos = 0;
    start_leaf.end_pos = N;
    if (pthread_create(&start_thread, NULL, threadFunction, (void *) &start_leaf) != 0)
        DEATH("pthread_create failed\n")

    // wait for the thread to finish
    if (pthread_join(start_thread, NULL) != 0) DEATH("pthread_join failed\n")

    // print the sorted array
    printf("the sorted array is:\n");
    for (int i = 0; i < N; i++) {
        printf("%d ", random_array[i]);
    }

    return 0;
}
