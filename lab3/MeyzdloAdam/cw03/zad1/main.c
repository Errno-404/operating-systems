#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>

// TODO error checking etc

int main(int argc, char * argv[])
{

    if(argc != 2){
        return 1;
    }

    int n = atoi(argv[1]);

    for (int i = 0; i < n; i++)
    {
        if (fork() == 0)
        {
            printf("parent pid: %d\tchild pid: %d\n", getppid(), getpid());
            exit(0);
        }
    }


    for (int i = 0; i < n; i++) // loop will run n times (n=5)
        wait(NULL);
    
    printf("\nargv[1] = %d\n", n);
    
}
