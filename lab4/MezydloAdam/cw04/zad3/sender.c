#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>

#define SIGNAL SIGUSR1
#define INITIAL_STATE 2

volatile int state_ack = INITIAL_STATE;

void signal_handler(int signo)
{
    state_ack++;
}

bool register_signal_handler()
{
    struct sigaction sa = {0};
    sa.sa_handler = signal_handler;

    if (sigaction(SIGNAL, &sa, NULL) == -1) {
        perror("[SENDER] Error registering signal handler");
        return false;
    }

    return true;
}

bool send_signal(int catcher_pid, int state)
{
    sigval_t sig_val = {state};
    if (sigqueue(catcher_pid, SIGNAL, sig_val) == -1) {
        perror("[SENDER] Error sending signal");
        return false;
    }

    printf("[SENDER] Sent state (%d).\n", state);
    return true;
}

bool wait_for_ack(int state_index)
{
    time_t start_time = clock();
    while (state_ack <= state_index)
    {
        if (clock() - start_time > CLOCKS_PER_SEC * 5)
        {
            printf("[SENDER] No confirmation, retrying.\n");
            return false;
        }
    }

    printf("[SENDER] Received confirmation.\n");
    return true;
}

int main(int argc, char **argv)
{
    // Check for valid arguments
    if (argc < 3)
    {
        fprintf(stderr, "[SENDER] Invalid arguments.\n");
        exit(EXIT_FAILURE);
    }

    // Get catcher PID from arguments
    int catcher_pid = atoi(argv[1]);

    // Loop through all the states and send signals
    for (int i = INITIAL_STATE; i < argc; i++)
    {
        int state = atoi(argv[i]);

        // Register signal handler
        if (!register_signal_handler())
            exit(EXIT_FAILURE);

        // Send signal to catcher
        if (!send_signal(catcher_pid, state))
            exit(EXIT_FAILURE);

        // Wait for acknowledgement
        if (!wait_for_ack(i))
            i--;
    }

    return EXIT_SUCCESS;
}
