#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/wait.h>

#define SIGNAL SIGUSR1
#define MAX_CALLS 4


int call_id;
int call_depth;

typedef void (*handler_t)(int, siginfo_t *, void *);

void set_action(struct sigaction action, handler_t handler, int flag);
void nodefer_handler(int signo, siginfo_t *info, void *context);
// SIGINFO
void info_handler(int signo, siginfo_t *info, void *context);
void nodefer_test(struct sigaction act);
void info_test(struct sigaction act);
void reset_test(struct sigaction act);
void reset_handler(int signo, siginfo_t *info, void *context);

int main(int argc, char **argv)
{
    if (argc != 1)
    {
        printf("Didn't expect any argument!\n");
        return 1;
    }

    struct sigaction action;

    // Testing SIGINFO HERE:
    printf("SIGINFO:\n");
    info_test(action);
    printf("\n");

    printf("NODEFER:\n");
    nodefer_test(action);
    printf("\n");

    printf("RESETHAND:\n");
    reset_test(action);

    return 0;
}

void set_action(struct sigaction action, handler_t handler, int flag)
{
    sigemptyset(&action.sa_mask);
    action.sa_sigaction = handler;
    action.sa_flags = flag;
    sigaction(SIGNAL, &action, NULL);
    call_id = 0;
    call_depth = 0;
}




// SIGINFO
void info_handler(int signo, siginfo_t *info, void *context)
{
    printf("[I HANDLER] Signal number:%d\n", info->si_signo);
    printf("[I HANDLER] PID:%d\n", info->si_pid);
    printf("[I HANDLER] UID:%d\n", info->si_uid);
    printf("[I HANDLER] USER TIME:%ld\n", info->si_utime);
    printf("[I HANDLER] SYSTEM TIME: %ld\n", info->si_stime);

    if(!(info->si_value.sival_ptr == NULL)){
        printf("[I HANDLER] VALUE PASSED: %d\n", info->si_value.sival_int);
    }
    

}
// NODEFER flag does not prevent handler function from sending the same signal it is currently handling, that's why something like recursive call happens here
void nodefer_handler(int signo, siginfo_t *info, void *context)
{
    printf("[N HANDLER] start: %d, call depth: %d\n", call_id, call_depth);

    call_id++;
    call_depth++;
    if (call_id < MAX_CALLS)
        raise(SIGNAL);
    call_depth--;

    printf("[N HANDLER] end:   %d, call depth: %d\n", call_id - 1, call_depth);
}
void reset_handler(int signo, siginfo_t *info, void *context){
    printf("[R HANDLER]\n");
}


void info_test(struct sigaction act)
{
    printf("=========SELF=========\n");
    set_action(act, info_handler, SA_SIGINFO);
    raise(SIGNAL);
    printf("\n");

    printf("=========CHILD=========\n");
    if (fork() == 0)
    {
        raise(SIGNAL);
        exit(0);
    }
    else
        wait(NULL);
    printf("\n");

    printf("=========CUSTOM=========\n");
    set_action(act, info_handler, SA_SIGINFO);

    union sigval value;
    value.sival_int = 2137;
    sigqueue(getpid(), SIGNAL, value);
}

void nodefer_test(struct sigaction act)
{
    set_action(act, nodefer_handler, SA_NODEFER);
    raise(SIGNAL);
}



void reset_test(struct sigaction act)
{
    set_action(act, reset_handler, SA_RESETHAND);
    raise(SIGNAL);
    raise(SIGNAL);
}

