/*************************************** Author : Mohamed Ayman ***************************************/
/*************************************** Name   : Sender.c      ***************************************/
/*************************************** Date   : 12-12-2024    ***************************************/
/*************************************** Version: 0.0.2         ***************************************/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <receiver_pid> <data>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    pid_t receiver_pid = atoi(argv[1]);
    int data = atoi(argv[2]);

    if (data != 0 && data != 1) {
        fprintf(stderr, "Data must be 0 or 1.\n");
        exit(EXIT_FAILURE);
    }

    // Create a signal value
    union sigval value;
    value.sival_int = data;

    // Send the signal
    if (sigqueue(receiver_pid, SIGRTMIN, value) == -1) {
        perror("sigqueue");
        exit(EXIT_FAILURE);
    }

    printf("Sent data %d to PID %d\n", data, receiver_pid);

    return 0;
}

