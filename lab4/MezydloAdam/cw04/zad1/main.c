#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <wait.h>

enum Mode
{
    MODE_IGNORE,
    MODE_HANDLER,
    MODE_MASK,
    MODE_PENDING,
    MODE_UNKNOWN
};


enum Mode parse_mode(const char* command);

const char* USAGE = "Usage: ./main <ignore|handler|mask|pending>\n";
const int SIGNAL_ID = SIGUSR1;

void handle_signal(int sigtype);
void mask_signal();
void check_signal_pending();


int main(int argc, char** argv)
{
    if (argc != 2)
    {
        printf("%s", USAGE);
        return 1;
    }

    const enum Mode mode = parse_mode(argv[1]);

    switch (mode)
    {
        case MODE_IGNORE:
            signal(SIGNAL_ID, SIG_IGN);
            printf("Ignoring the signal...\n");
            break;

        case MODE_HANDLER:
            signal(SIGNAL_ID, handle_signal);
            printf("Handling the signal with a custom handler...\n");
            break;

        case MODE_MASK:
            mask_signal();
            printf("Masking the signal...\n");
            break;

        case MODE_PENDING:
            mask_signal();
            printf("Checking if the signal is pending...\n");
            check_signal_pending();
            break;

        default:
            printf("Unknown mode: %s\n", argv[1]);
            return 1;
    }

    printf("Raising the signal in the main process...\n");
    raise(SIGNAL_ID);
    printf("Signal raised in the main process.\n");

    int pid = fork();
    if (pid == -1)
    {
        fprintf(stderr, "Failed to fork.");
        exit(1);
    }

    else if (pid == 0)
    {
        if (mode != MODE_PENDING)
        {
            printf("Raising the signal in the child process...\n");
            raise(SIGNAL_ID);
            printf("Signal raised in the child process.\n");
        }

        printf("Checking if the signal is pending in the child process...\n");
        check_signal_pending();
        printf("Starting the sub-program 'other'...\n");

        if (execl("./other", "other", NULL) == -1)
        {
            fprintf(stderr, "Couldn't run sub-program 'other'. Make sure to compile it first.");
            exit(1);
        }
    }

    int status = 0;
    while (wait(&status) > 0);

    return 0;
}

enum Mode parse_mode(const char* command)
{
    if (strcmp(command, "ignore") == 0)
    {
        return MODE_IGNORE;
    }
    else if (strcmp(command, "handler") == 0)
    {
        return MODE_HANDLER;
    }
    else if (strcmp(command, "mask") == 0)
    {
        return MODE_MASK;
    }
    else if (strcmp(command, "pending") == 0)
    {
        return MODE_PENDING;
    }
    else return MODE_UNKNOWN;
}

void handle_signal(int sigtype)
{
    printf("Received signal %d\n", sigtype);
}

void mask_signal()
{
    sigset_t blocked_set;
    sigemptyset(&blocked_set);
    sigaddset(&blocked_set, SIGNAL_ID);
    sigprocmask(SIG_BLOCK, &blocked_set, NULL);
}

void check_signal_pending()
{
    sigset_t pending_set;
    sigpending(&pending_set);

    if (sigismember(&pending_set, SIGNAL_ID))
    {
        printf("Signal with id %d is pending.\n", SIGNAL_ID);
    }
    else
    {
        printf("Signal with id %d is NOT pending.\n", SIGNAL_ID);
    }
}
