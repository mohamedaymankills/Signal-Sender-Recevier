#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

// Signal handler
void handle_signal(int sig, siginfo_t *info, void *context) {
    if (sig == SIGRTMIN) {
        int data = info->si_value.sival_int;
        printf("Received signal with data: %d\n", data);
        if (data == 0) {
            printf("Terminating gracefully...\n");
            exit(0);
        } else if (data == 1) {
            printf("Aborting with core dump...\n");
            abort();
        }
    }
}

int main() {
    struct sigaction sa;

    // Set up signal handler
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = handle_signal;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGRTMIN, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    printf("Receiver PID: %d\n", getpid());
    printf("Waiting for signals...\n");

    // Infinite loop to keep the process alive
    while (1) {
        pause(); // Wait for signals
    }

    return 0;
}

