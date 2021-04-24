#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#define PROCESS_NUM 10
#define ALARM_RANGE 10
#define ALARM_MIN 1
#define DEATH(message)      \
    {                       \
        perror(message);    \
        exit(EXIT_FAILURE); \
    }


void quit_handler(int signum) {
    exit(EXIT_SUCCESS);
}


void alarm_handler(int signum) {

    int alarm_time;


    // print the message
    printf("I am process %d and I am alive\n", getpid());

    // install the signal for SIGALARM
    signal(SIGALRM, alarm_handler);

    // generate the random alarm
    alarm_time = (int) (random() % ALARM_RANGE + ALARM_MIN);
    alarm(alarm_time);
}


int main() {

    pid_t pids[PROCESS_NUM], dying_pid;
    int remaining_children, alarm_time;


    // create the processes
    remaining_children = PROCESS_NUM;
    for (int i = 0; i < PROCESS_NUM; i++) {
        pids[i] = fork();
        if (pids[i] == -1) DEATH("fork failed")
        else if (pids[i] == 0) {
            // install the signal for SIGINT to terminate the process
            signal(SIGINT, quit_handler);

            // install the signal for SIGALARM
            signal(SIGALRM, alarm_handler);

            // initialize random number generator
            srandom(getpid());

            // generate the random alarm
            alarm_time = (int) (random() % ALARM_RANGE + ALARM_MIN);
            alarm(alarm_time);

            while (1) { sleep(1); }
        }
    }

    // loop to interact with the user
    while (remaining_children > 0) {
        // print the remaining_children
        printf("remaining_children are:\n");
        for (int i = 0; i < remaining_children; i++) { printf("%d ", pids[i]); }
        printf("\nwhich one do you want to eliminate:\n");
        scanf("%d", &dying_pid);// NOLINT(cert-err34-c)

        // check if inserted pid is valid and kill it
        for (int i = 0; i < remaining_children; i++) {
            if (dying_pid == pids[i]) {
                if (kill(dying_pid, SIGINT) == -1) DEATH("kill failed")
                wait(NULL);

                // substitute the eliminated pid with another available
                pids[i] = pids[remaining_children - 1];
                remaining_children--;
            }
        }
    }

    return 0;
}
