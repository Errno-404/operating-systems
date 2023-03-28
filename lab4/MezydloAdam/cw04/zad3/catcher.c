#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/times.h>

typedef enum {
    ONE_HUNDRED = 1,
    TIME_STAMP = 2,
    REQUEST_COUNT = 3,
    TIME_LOOP = 4,
    SHUT_DOWN = 5,
    DO_NOTHING = 6
} MODE;

MODE mode = DO_NOTHING;
bool completed = true;
int request_counter = 0;

void single_time_stamp();
void print_one_hundred();
void print_request_counter();
void time_stamp_loop();
void shut_down();
void handler(int signo, siginfo_t *info, void *context);


int main() {
    struct sigaction action;
    sigemptyset(&action.sa_mask);
    action.sa_sigaction = handler;
    action.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &action, NULL);

    printf("[CATCHER] Starting, PID= %d\n", getpid());

    while (true) {
        
        if (completed)
            {
                // causes catcher to wait for signals!
                pause();
            }

        switch (mode) {
            case ONE_HUNDRED:
                print_one_hundred();
                break;
            case TIME_STAMP:
                single_time_stamp();
                break;
            case REQUEST_COUNT:
                print_request_counter();
                break;
            case TIME_LOOP:
                time_stamp_loop();
                break;
            case SHUT_DOWN:
                shut_down();
                break;
            default:
                completed = true;
        }

        if (mode != TIME_LOOP)
            completed = true;
    }

    return 0;
}

void single_time_stamp() {
    time_t raw_time = time(NULL);
    struct tm *time_info = localtime(&raw_time);
    printf("[CATCHER] Current timestamp: %s", asctime(time_info));
    fflush(NULL);
}

void print_one_hundred() {
    for (int i = 0; i < 100; i++)
        printf("[CATCHER] %i\n", i + 1);
    fflush(NULL);
}

void print_request_counter() {
    printf("[CATCHER] Number of valid requests: %d\n", request_counter);
    fflush(NULL);
}

void time_stamp_loop() {
    clock_t start = clock();
    double elapsed_time;
    while (mode == TIME_LOOP) {
        elapsed_time = ((double)(clock() - start)) / CLOCKS_PER_SEC;
        if (elapsed_time >= 1.0) {
            single_time_stamp();
            start = clock();
        }
    }
}

void shut_down() {
    printf("[CATCHER] Ending.\n");
    fflush(NULL);
    exit(0);
}

void handler(int signo, siginfo_t *info, void *context) {
    int sender = info->si_pid;
    int request = info->si_value.sival_int;

    if (request >= 1 && request <= 5) {
        mode = (MODE)request;
        request_counter++;
        completed = false;
    }
    else {
        printf("[CATCHER] Expected request in range: 1..5, got %d instead!\n", request);
        fflush(NULL);
        mode = DO_NOTHING;
        completed = true;
    }

    kill(sender, SIGUSR1);
}
