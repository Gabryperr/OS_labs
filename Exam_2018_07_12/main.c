#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#define TRUE 1
#define FALSE 0

#define MSG_LENGTH 128
#define MSG_EQUAL 0
#define END_MESSAGE "end"

#define INPUT_FILE "data.dat"
#define OUTPUT_FILE "output.dat"

#define FAIL -1
#define SUCCESS 0
#define FORK_CHILD 0

#define MIN 1.000

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

typedef struct msgbuf {
    long mtype;
    char mtext[MSG_LENGTH];
} msgbuf_t;

int finish;
double shared_num;
sem_t sem_read, sem_write;

void *threadFunction() {
    FILE *fp;

    // open file
    if ((fp = fopen(OUTPUT_FILE, "w")) == NULL)
        PDEATH("failed to open output file\n")

    while (!finish) {
        if (sem_wait(&sem_write) == FAIL)
            PDEATH("failed to wait on sem write\n")
        if (!finish) {
            fprintf(fp, "%lf\n", shared_num);
            if (sem_post(&sem_read) == FAIL)
                PDEATH("failed to post on sem read\n")
        }
    }

    // close file
    if (fclose(fp) == EOF)
        PDEATH("failed to close file\n")

    pthread_exit(EXIT_SUCCESS);
}

int main() {

    pid_t pid;
    int msg_id;
    msgbuf_t msg;
    double new_num;
    FILE *fp;
    pthread_t thread;

    // create message queue to communicate between parent and child
    if ((msg_id = msgget(IPC_PRIVATE, S_IRWXU | IPC_CREAT)) == FAIL)
        PDEATH("failed to crate message queue\n")
    // clean the queue
    while (msgrcv(msg_id, &msg, MSG_LENGTH, 0, IPC_NOWAIT) != FAIL)
        ;

    // fork the program
    if ((pid = fork()) == FAIL)
        PDEATH("fork failed\n")

    if (pid == FORK_CHILD) {// child process

        // init semaphore
        if (sem_init(&sem_read, 0, 1) == FAIL)
            PDEATH("sem_init failed\n")
        if (sem_init(&sem_write, 0, 0) == FAIL)
            PDEATH("sem_init failed\n")
        finish = FALSE;

        // create thread
        if (pthread_create(&thread, NULL, threadFunction, NULL) != SUCCESS)
            DEATH("failed to create thread\n")

        // read message from parent and sent number to other thread
        do {
            if (msgrcv(msg_id, &msg, MSG_LENGTH, 0, 0) == FAIL)
                PDEATH("failed to receive message\n")
            if (strcmp(msg.mtext, END_MESSAGE) != MSG_EQUAL) {
                if (sem_wait(&sem_read) == FAIL)
                    PDEATH("failed to wait on sem read\n")
                sscanf(msg.mtext, "%lf", &shared_num);
                if (sem_post(&sem_write) == FAIL)
                    PDEATH("failed to post on sem write\n")
            }
        } while (strcmp(msg.mtext, END_MESSAGE) != MSG_EQUAL);

        // signal thread to finish
        if (sem_wait(&sem_read) == FAIL)
            PDEATH("failed to wait on sem read\n")
        finish = TRUE;
        if (sem_post(&sem_write) == FAIL)
            PDEATH("failed to post on sem write\n")

        // join thread
        if (pthread_join(thread, NULL) != SUCCESS)
            DEATH("failed to join thread\n")

        // destroy semaphore
        if (sem_destroy(&sem_read) == FAIL)
            PDEATH("failed to destroy sem read\n")
        if (sem_destroy(&sem_write) == FAIL)
            PDEATH("failed to destroy sem write\n")

    } else {// parent process

        // open file
        if ((fp = fopen(INPUT_FILE, "r")) == NULL)
            PDEATH("failed to open input file\n")

        // read the file and send numbers to child
        while (!feof(fp)) {
            fscanf(fp, "%lf", &new_num);
            if (new_num > MIN) {
                msg.mtype = 1;
                sprintf(msg.mtext, "%lf", new_num);
                if (msgsnd(msg_id, &msg, MSG_LENGTH, 0) == FAIL)
                    PDEATH("failed to send message to child\n")
            }
        }
        // send finish message to child
        msg.mtype = 1;
        strcpy(msg.mtext, END_MESSAGE);
        if (msgsnd(msg_id, &msg, MSG_LENGTH, 0) == FAIL)
        PDEATH("failed to send message to child\n")

        // close file
        if (fclose(fp) == EOF)
        PDEATH("failed to close file\n")

        // wait for child to finish
        if (wait(NULL) == FAIL)
        PDEATH("failed to wait for child\n")
    }

    return 0;
}
