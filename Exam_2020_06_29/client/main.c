#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <unistd.h>
#define ELEMENTS 6
#define FALSE 0

#define SHM_FAIL -1
#define MSG_FAIL -1
#define STR_EQUAL 0

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
#define CONNECT "connect"
#define GUESS "guess"
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

    int player, elements_id, guess_id, score_id, *guess, *score, msg_id;
    element_t *elements_array;
    msgbuf_t msgbuf;

    // get player number
    printf("insert player number 0 or 1\n");
    scanf("%d", &player);

    // connect to shared memory for data and visibility
    if ((elements_id = shmget(ELEMENTS_KEY, sizeof(element_t) * ELEMENTS * 2, S_IRWXU)) == SHM_FAIL)
        PDEATH("failed to connect shared memory for elements\n")
    if ((elements_array = (element_t *) shmat(elements_id, NULL, 0)) == (element_t *) SHM_FAIL)
        PDEATH("failed to attach shared memory for elements\n")

    // connect the shared memory for guess
    if ((guess_id = shmget(GUESS_KEY, sizeof(int) * 2, S_IRWXU)) == SHM_FAIL)
        PDEATH("failed to connect shared memory for guess\n")
    if ((guess = (int *) shmat(guess_id, NULL, 0)) == (int *) SHM_FAIL)
        PDEATH("failed to attach shared memory for guess\n")

    // connect shared memory for score
    if ((score_id = shmget(SCORE_KEY, sizeof(int) * 2, S_IRWXU)) == SHM_FAIL)
        PDEATH("failed to connect shared memory for score\n")
    if ((score = (int *) shmat(score_id, NULL, 0)) == (int *) SHM_FAIL)
        PDEATH("failed to attach shared memory for score\n")

    // connect message queues
    if (player == 0) {
        if ((msg_id = msgget(MSG_KEY_0, S_IRWXU | IPC_CREAT)) == MSG_FAIL)
            PDEATH("failed to connect message queue\n")
    } else {
        if ((msg_id = msgget(MSG_KEY_1, S_IRWXU | IPC_CREAT)) == MSG_FAIL)
            PDEATH("failed to connect message queue \n")
    }

    // send message to server to signal presence
    strcpy(msgbuf.mtext, CONNECT);
    msgbuf.mtype = MSG_FROM_CLIENT;
    if (msgsnd(msg_id, &msgbuf, MSG_LENGTH, 0) == MSG_FAIL)
        PDEATH("failed to send message to server\n")

    do {
        // wait for message from server
        if (msgrcv(msg_id, &msgbuf, MSG_LENGTH, MSG_FROM_SERVER, 0) == MSG_FAIL)
            PDEATH("failed to receive message from server\n")

        // check which message
        if (strcmp(msgbuf.mtext, SHOW_CURRENT) == STR_EQUAL) {
            // show score and current game situation
            system("clear");
            printf("Current score:\n\n");
            printf("\tYour score: %d \t\t Your opponent score: %d\n\n",
                   score[player], score[(player + 1) % 2]);
            printf("Game status:\n\n");
            printf("\t");
            for (int i = 0; i < ELEMENTS * 2; i++) {
                printf("%d", i / 10);
            }
            printf("\n\t");
            for (int i = 0; i < ELEMENTS * 2; i++) {
                printf("%d", i % 10);
            }
            printf("\n\t");
            for (int i = 0; i < ELEMENTS * 2; i++) {
                if (elements_array[i].visible) {
                    printf("%c", elements_array[i].letter);
                } else {
                    printf(".");
                }
            }
            printf("\n");

        } else if (strcmp(msgbuf.mtext, ASK) == STR_EQUAL) {
            // ask for guess
            printf("\n\nIt's yor turn, make your guess! Insert first number 0-51\n");
            scanf("%d", &guess[0]);
            printf("Insert second number 0-51\n");
            scanf("%d", &guess[1]);

            // send message to client to inform that the guess is ready
            strcpy(msgbuf.mtext, GUESS);
            msgbuf.mtype = MSG_FROM_CLIENT;
            if (msgsnd(msg_id, &msgbuf, MSG_LENGTH, 0) == MSG_FAIL)
                PDEATH("failed to send message to server\n")

        } else if (strcmp(msgbuf.mtext, WAIT) == STR_EQUAL) {
            // tell to wait
            printf("\n\nIt's your opponent turn, wait\n");

        } else if (strcmp(msgbuf.mtext, SHOW_MISPREDICT) == STR_EQUAL) {
            // show misprediction
            system("clear");
            printf("Current score:\n\n");
            printf("\tYour score: %d \t\t Your opponent score: %d\n\n",
                   score[player], score[(player + 1) % 2]);
            printf("You have made a wrong choice:\n\n");
            printf("\t");
            for (int i = 0; i < ELEMENTS * 2; i++) {
                printf("%d", i / 10);
            }
            printf("\n\t");
            for (int i = 0; i < ELEMENTS * 2; i++) {
                printf("%d", i % 10);
            }
            printf("\n\t");
            for (int i = 0; i < ELEMENTS * 2; i++) {
                if (elements_array[i].visible) {
                    printf("%c", elements_array[i].letter);
                } else {
                    printf(".");
                }
            }
            printf("\n");

            // wait 10 seconds
            sleep(4);
            elements_array[guess[0]].visible = FALSE;
            elements_array[guess[1]].visible = FALSE;


        } else if (strcmp(msgbuf.mtext, SHOW_MISPREDICT_OTHER) == STR_EQUAL) {
            // show misprediction
            system("clear");
            printf("Current score:\n\n");
            printf("\tYour score: %d \t\t Your opponent score: %d\n\n",
                   score[player], score[(player + 1) % 2]);
            printf("Your opponent has made a wrong choice:\n\n");
            printf("\t");
            for (int i = 0; i < ELEMENTS * 2; i++) {
                printf("%d", i / 10);
            }
            printf("\n\t");
            for (int i = 0; i < ELEMENTS * 2; i++) {
                printf("%d", i % 10);
            }
            printf("\n\t");
            for (int i = 0; i < ELEMENTS * 2; i++) {
                if (elements_array[i].visible) {
                    printf("%c", elements_array[i].letter);
                } else {
                    printf(".");
                }
            }
            printf("\n");

            // wait 10 seconds
            sleep(4);
            elements_array[guess[0]].visible = FALSE;
            elements_array[guess[1]].visible = FALSE;

        }
    } while (strcmp(msgbuf.mtext, WIN) != STR_EQUAL &&
             strcmp(msgbuf.mtext, LOOSE) != STR_EQUAL &&
             strcmp(msgbuf.mtext, TIE) != STR_EQUAL);


    // check finish situation from message
    if (strcmp(msgbuf.mtext, WIN) == STR_EQUAL) {
        printf("\n\nYou have won the match!\n");
    } else if (strcmp(msgbuf.mtext, LOOSE) == STR_EQUAL) {
        printf("\n\nYou have lost the match!\n");
    } else if (strcmp(msgbuf.mtext, TIE) == STR_EQUAL) {
        printf("\n\nThe match has finished in a tie\n");
    }

    // detach all shared memory
    if (shmdt(elements_array) == SHM_FAIL)
        PDEATH("failed to detach shared memory for array\n")
    if (shmdt(guess) == SHM_FAIL)
        PDEATH("failed to detach shared memory for guess\n")
    if (shmdt(score) == SHM_FAIL)
        PDEATH("failed to detach shared memory for score\n")

    return 0;
}
