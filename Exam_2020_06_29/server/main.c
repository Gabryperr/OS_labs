#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <time.h>
#define ELEMENTS 6
#define TRUE 1
#define FALSE 0

#define SHM_FAIL -1
#define MSG_FAIL -1

//keys
#define ELEMENTS_KEY 1
#define SCORE_KEY 2
#define GUESS_KEY 3
#define MSG_KEY_0 10
#define MSG_KEY_1 20

// messages
#define SHOW_CURRENT "show current"
#define ASK "ask"
#define WAIT "wait"
#define SHOW_MISPREDICT "show mispredict"
#define SHOW_MISPREDICT_OTHER "show mispredict other"
#define WIN "win"
#define LOOSE "loose"
#define TIE "tie"
#define MSG_LENGTH 32
#define MSG_FROM_SERVER 1
#define MSG_FROM_CLIENT 2

#define PDEATH(message)     \
    {                       \
        perror(message);    \
        exit(EXIT_FAILURE); \
    }

typedef struct msgbuf_struct {
    long mtype;
    char mtext[MSG_LENGTH];
} msgbuf_t;

typedef struct element_struct {
    char letter;
    int visible;
} element_t;

int main() {

    int elements_id, occurrences[ELEMENTS], rand_element, msg_1_id, msg_2_id, *score, score_id, turn, *guess, guess_id;
    element_t *elements_array;
    msgbuf_t msgbuf_1, msgbuf_2;

    // create the shared memory containing data and visibility
    if ((elements_id = shmget(ELEMENTS_KEY, sizeof(element_t) * ELEMENTS * 2, S_IRWXU | IPC_CREAT)) == SHM_FAIL)
        PDEATH("failed to create shared memory for elements\n")
    if ((elements_array = (element_t *) shmat(elements_id, NULL, 0)) == (element_t *) SHM_FAIL)
        PDEATH("failed to attach shared memory for elements\n")
    printf("create shared memory for elements\n");

    // initialize occurrences to zero
    for (int i = 0; i < ELEMENTS; i++) {
        occurrences[i] = 0;
    }
    printf("initialize occurrences\n");

    // populate elements array casually
    srandom(time(NULL));
    for (int i = 0; i < ELEMENTS * 2; i++) {
        do {
            rand_element = (int) (random() % ELEMENTS);
        } while (occurrences[rand_element] == 2);
        occurrences[rand_element]++;
        elements_array[i].letter = 'A' + rand_element;
        elements_array[i].visible = FALSE;
    }
    printf("created game\n");

    // create the shared memory for guess
    if ((guess_id = shmget(GUESS_KEY, sizeof(int) * 2, S_IRWXU | IPC_CREAT)) == SHM_FAIL)
        PDEATH("failed to create shared memory for guess\n")
    if ((guess = (int *) shmat(guess_id, NULL, 0)) == (int *) SHM_FAIL)
        PDEATH("failed to attach shared memory for guess\n")
    printf("create shared memory for guess\n");

    // create shared memory for score
    if ((score_id = shmget(SCORE_KEY, sizeof(int) * 2, S_IRWXU | IPC_CREAT)) == SHM_FAIL)
        PDEATH("failed to create shared memory for score\n")
    if ((score = (int *) shmat(score_id, NULL, 0)) == (int *) SHM_FAIL)
        PDEATH("failed to attach shared memory for score\n")
    printf("create shared memory for score\n");

    //initialize score
    score[0] = 0;
    score[1] = 0;

    // assign first turn to player 1
    turn = 0;

    // crate message queues
    if ((msg_1_id = msgget(MSG_KEY_0, S_IRWXU | IPC_CREAT)) == MSG_FAIL)
        PDEATH("failed to create message queue 1\n")
    if ((msg_2_id = msgget(MSG_KEY_1, S_IRWXU | IPC_CREAT)) == MSG_FAIL)
        PDEATH("failed to create message queue 2\n")
    printf("create message queue\n");

    while (msgrcv(msg_1_id, &msgbuf_1, MSG_LENGTH, 0, IPC_NOWAIT) != MSG_FAIL)
        ;
    while (msgrcv(msg_2_id, &msgbuf_2, MSG_LENGTH, 0, IPC_NOWAIT) != MSG_FAIL)
        ;
    printf("clean message queues\n");

    //wait for clients to connect
    if (msgrcv(msg_1_id, &msgbuf_1, MSG_LENGTH, MSG_FROM_CLIENT, 0) == MSG_FAIL)
        PDEATH("failed to connect to client 1\n")
    printf("client 1 connect\n");

    if (msgrcv(msg_2_id, &msgbuf_2, MSG_LENGTH, MSG_FROM_CLIENT, 0) == MSG_FAIL)
        PDEATH("failed to connect to client 2\n")
    printf("client 2 connect\n");

    // begin loop to alternate turns
    while (score[0] <= ELEMENTS / 2 && score[1] <= ELEMENTS / 2 && score[0] + score[1] < ELEMENTS) {

        //send message to both client to show current situation
        strcpy(msgbuf_1.mtext, SHOW_CURRENT);
        msgbuf_1.mtype = MSG_FROM_SERVER;
        if (msgsnd(msg_1_id, &msgbuf_1, MSG_LENGTH, 0) == MSG_FAIL)
            PDEATH("failed to send message to client 1\n")

        strcpy(msgbuf_2.mtext, SHOW_CURRENT);
        msgbuf_2.mtype = MSG_FROM_SERVER;
        if (msgsnd(msg_2_id, &msgbuf_2, MSG_LENGTH, 0) == MSG_FAIL)
            PDEATH("failed to send message to client 2\n")

        if (turn == 0) {
            // send player 1 ask message and player 2 wait message
            strcpy(msgbuf_1.mtext, ASK);
            msgbuf_1.mtype = MSG_FROM_SERVER;
            if (msgsnd(msg_1_id, &msgbuf_1, MSG_LENGTH, 0) == MSG_FAIL)
                PDEATH("failed to send message to client 1\n")

            strcpy(msgbuf_2.mtext, WAIT);
            msgbuf_2.mtype = MSG_FROM_SERVER;
            if (msgsnd(msg_2_id, &msgbuf_2, MSG_LENGTH, 0) == MSG_FAIL)
                PDEATH("failed to send message to client 2\n")

            // wait for guess from player 1
            if (msgrcv(msg_1_id, &msgbuf_1, MSG_LENGTH, MSG_FROM_CLIENT, 0) == MSG_FAIL)
                PDEATH("failed to receive guess from client 1\n")

            // check guess from player 1
            if (guess[0] < ELEMENTS * 2 && guess[1] < ELEMENTS * 2 && guess[0] != guess[1]) {// if guess are in range
                if (!elements_array[guess[0]].visible && !elements_array[guess[1]].visible) {// if are not already visible
                    elements_array[guess[0]].visible = TRUE;
                    elements_array[guess[1]].visible = TRUE;
                    if (elements_array[guess[0]].letter == elements_array[guess[1]].letter) {// correct prediction
                        score[turn]++;

                    } else {//misprediction
                        // send message to both to show misprediction
                        strcpy(msgbuf_1.mtext, SHOW_MISPREDICT);
                        msgbuf_1.mtype = MSG_FROM_SERVER;
                        if (msgsnd(msg_1_id, &msgbuf_1, MSG_LENGTH, 0) == MSG_FAIL)
                            PDEATH("failed to send message to client 1\n")

                        strcpy(msgbuf_2.mtext, SHOW_MISPREDICT_OTHER);
                        msgbuf_2.mtype = MSG_FROM_SERVER;
                        if (msgsnd(msg_2_id, &msgbuf_2, MSG_LENGTH, 0) == MSG_FAIL)
                            PDEATH("failed to send message to client 2\n")

                    }
                }
            }

            // change turn
            turn = 1;

        } else {
            // send player 2 ask message and player 1 wait message
            strcpy(msgbuf_1.mtext, WAIT);
            msgbuf_1.mtype = MSG_FROM_SERVER;
            if (msgsnd(msg_1_id, &msgbuf_1, MSG_LENGTH, 0) == MSG_FAIL)
                PDEATH("failed to send message to client 1\n")

            strcpy(msgbuf_2.mtext, ASK);
            msgbuf_2.mtype = MSG_FROM_SERVER;
            if (msgsnd(msg_2_id, &msgbuf_2, MSG_LENGTH, 0) == MSG_FAIL)
                PDEATH("failed to send message to client 2\n")

            // wait for guess from player 2
            if (msgrcv(msg_2_id, &msgbuf_2, MSG_LENGTH, MSG_FROM_CLIENT, 0) == MSG_FAIL)
                PDEATH("failed to receive guess from client 2\n")

            // check guess from player 2
            if (guess[0] < ELEMENTS * 2 && guess[1] < ELEMENTS * 2 && guess[0] != guess[1]) {                        // if guess are in range
                if (!elements_array[guess[0]].visible && !elements_array[guess[1]].visible) {// if are not already visible
                    elements_array[guess[0]].visible = TRUE;
                    elements_array[guess[1]].visible = TRUE;
                    if (elements_array[guess[0]].letter == elements_array[guess[1]].letter) {// correct prediction
                        score[turn]++;

                    } else {//misprediction
                        // send message to both to show misprediction
                        strcpy(msgbuf_1.mtext, SHOW_MISPREDICT_OTHER);
                        msgbuf_1.mtype = MSG_FROM_SERVER;
                        if (msgsnd(msg_1_id, &msgbuf_1, MSG_LENGTH, 0) == MSG_FAIL)
                            PDEATH("failed to send message to client 1\n")

                        strcpy(msgbuf_2.mtext, SHOW_MISPREDICT);
                        msgbuf_2.mtype = MSG_FROM_SERVER;
                        if (msgsnd(msg_2_id, &msgbuf_2, MSG_LENGTH, 0) == MSG_FAIL)
                            PDEATH("failed to send message to client 2\n")

                    }
                }
            }

            // change turn
            turn = 0;
        }
    }

    // check result and inform clients
    if (score[0] > ELEMENTS / 2) {
        // send player 1 win and player 2 loose
        strcpy(msgbuf_1.mtext, WIN);
        msgbuf_1.mtype = MSG_FROM_SERVER;
        if (msgsnd(msg_1_id, &msgbuf_1, MSG_LENGTH, 0) == MSG_FAIL)
            PDEATH("failed to send message to client 1\n")

        strcpy(msgbuf_2.mtext, LOOSE);
        msgbuf_2.mtype = MSG_FROM_SERVER;
        if (msgsnd(msg_2_id, &msgbuf_2, MSG_LENGTH, 0) == MSG_FAIL)
            PDEATH("failed to send message to client 2\n")

        printf("player 0 win\n");

    } else if (score[1] > ELEMENTS / 2) {
        // send player 1 loose and player 2 win
        strcpy(msgbuf_1.mtext, LOOSE);
        msgbuf_1.mtype = MSG_FROM_SERVER;
        if (msgsnd(msg_1_id, &msgbuf_1, MSG_LENGTH, 0) == MSG_FAIL)
            PDEATH("failed to send message to client 1\n")

        strcpy(msgbuf_2.mtext, WIN);
        msgbuf_2.mtype = MSG_FROM_SERVER;
        if (msgsnd(msg_2_id, &msgbuf_2, MSG_LENGTH, 0) == MSG_FAIL)
            PDEATH("failed to send message to client 2\n")

        printf("player 1 win\n");

    } else {
        // send both player that is a tie
        strcpy(msgbuf_1.mtext, TIE);
        msgbuf_1.mtype = MSG_FROM_SERVER;
        if (msgsnd(msg_1_id, &msgbuf_1, MSG_LENGTH, 0) == MSG_FAIL)
            PDEATH("failed to send message to client 1\n")

        strcpy(msgbuf_2.mtext, TIE);
        msgbuf_2.mtype = MSG_FROM_SERVER;
        if (msgsnd(msg_2_id, &msgbuf_2, MSG_LENGTH, 0) == MSG_FAIL)
            PDEATH("failed to send message to client 2\n")

        printf("tie");
    }

    // detach all shared memory
    if (shmdt(elements_array) == SHM_FAIL)
        PDEATH("failed to detach shared memory for array\n")
    if (shmdt(guess) == SHM_FAIL)
        PDEATH("failed to detach shared memory for guess\n")
    if (shmdt(score) == SHM_FAIL)
        PDEATH("failed to detach shared memory for score\n")

    // remove all shared memory
    if (shmctl(elements_id, IPC_RMID, NULL) == SHM_FAIL)
        PDEATH("failed to remove shared memory for array\n")
    if (shmctl(guess_id, IPC_RMID, NULL) == SHM_FAIL)
        PDEATH("failed to remove shared memory for guess\n")
    if (shmctl(score_id, IPC_RMID, NULL) == SHM_FAIL)
        PDEATH("failed to remove shared memory for score\n")

    // close message queues
    if (msgctl(msg_1_id, IPC_RMID, NULL) == MSG_FAIL)
        PDEATH("failed to close message queue 1\n")
    if (msgctl(msg_2_id, IPC_RMID, NULL) == MSG_FAIL)
        PDEATH("failed to close message queue 2\n")

    return 0;
}
