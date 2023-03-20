#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
#include <ctype.h>

int isnum(char *);

void introduce_pid(int n);

int main(int argc, char *argv[])
{

    // checking for argument errors
    if (argc != 2)
    {
        printf("1 argumen expected, got %d instead!\n", argc - 1);
        return 1;
    }

    if (!isnum(argv[1]))
    {
        printf("%s is not a number!", argv[1]);
        return 1;
    }

    int n = atoi(argv[1]);
    introduce_pid(n);
    printf("\nargv[1] = %d\n", n);
}

int isnum(char *s)
{
    int str_n = strlen(s);
    for (int i = 0; i < str_n; ++i)
    {
        if (!isdigit(s[i]))
        {
            return 0;
        }
    }
    return 1;
}

void introduce_pid(int n)
{
    for (int i = 0; i < n; ++i)
    {

        // creates n processes with fork() then exits.
        int pid;
        if ((pid = fork()) == 0)
        {
            printf("parent pid: %d\tchild pid: %d\n", getppid(), getpid());
            exit(0);
        }
        else if (pid == -1)
        {
            printf("fork() returned -1!\n");
            abort();
        }
    }

    for (int i = 0; i < n; ++i)
    {
        if (wait(NULL) == -1)
        {
            printf("wiat() returned -1!\n");
            abort();
        }
    }
}
