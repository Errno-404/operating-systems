#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>


int parent = 1;

enum Mode
{
    MODE_IGNORE,
    MODE_HANDLER,
    MODE_MASK,
    MODE_PENDING,
    MODE_UNKNOWN
};

const char *USAGE = "Usage: ./main <ignore|handler|mask|pending>\n";
const char *OPTION = "[OPTION]:";

enum Mode parse_mode(const char *command);
void handle_signal(int sigtype);
void mask_signal();
void check_signal_pending();
void raise_signal();
void execute_task(enum Mode mode);

int main(int argc, char **argv)
{
    if (argc < 2 || argc > 3)
    {
        printf("Expected 1 argument, got %d - '%s' instead!\n", argc - 1, USAGE);
        return 1;
    }
    else if (argc == 3)
    {
        parent = 0;
    }

#ifdef EXEC
    if (parse_mode(argv[1]) == MODE_HANDLER)
    {
        printf("handler is unavailable with exec option!\n");
        exit(1);
    }
#endif

    const enum Mode mode = parse_mode(argv[1]);

    execute_task(mode);

#ifdef EXEC
    if (parent)
    {
        if (execl(argv[0], argv[0], argv[1], "reset", NULL) == -1)
        {
            fprintf(stderr, "Couldn't run this program once again!\n");
            exit(1);
        }
    }

#else
    parent = fork();
    if (!parent)
        execute_task(mode);
    else
        wait(NULL);

#endif
    return 0;
}

void execute_task(enum Mode mode)
{
    switch (mode)
    {
    case MODE_IGNORE:
        signal(SIGUSR1, SIG_IGN);
        raise_signal();
        break;

    case MODE_HANDLER:
        signal(SIGUSR1, handle_signal);
        raise_signal();
        break;

    case MODE_MASK:
        signal(SIGUSR1, handle_signal);
        if (parent)
        {
            mask_signal();
            raise_signal();
        }
        break;

    case MODE_PENDING:
        signal(SIGUSR1, handle_signal);
        if (parent)
        {
            mask_signal();
            raise_signal();
        }

        check_signal_pending();
        break;

    default:
        printf("Unknown mode\n");
        exit(1);
    }
}

void raise_signal()
{
    printf("(%d) Raising signal...\n", getpid());
    raise(SIGUSR1);
}

enum Mode parse_mode(const char *command)
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
    else
        return MODE_UNKNOWN;
}

void handle_signal(int sigtype)
{
    printf("(%d) Received signal %d\n", getpid(), sigtype);
}


void mask_signal()
{
    struct sigaction action;
    sigemptyset(&action.sa_mask);
    sigaddset(&action.sa_mask, SIGUSR1);
    sigprocmask(SIG_BLOCK, &action.sa_mask, NULL);
}


void check_signal_pending()
{
    sigset_t pending_set;
    sigpending(&pending_set);

    if (sigismember(&pending_set, SIGUSR1))
    {
        printf("(%d) Signal %d is pending.\n", getpid(), SIGUSR1);
    }
    else
    {
        printf("(%d) Signal %d is not pending.\n", getpid(), SIGUSR1);
    }
}