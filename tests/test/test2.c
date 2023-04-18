#include <stdio.h>
#include <unistd.h>


// co on dokładnie robi?




int main(){


    int fd[2], d;
    pipe(fd);

    if(fork() ==0){

        close(fd[0]);
        d = write(fd[1], "some", 4);
        printf("wyslano %d bajtów", d);
        return 0;
    }
    else{
        char buf[10];
        close(fd[1]);
        dup2(fd[0], STDIN_FILENO);
        execlp("grep", "grep", "ako", NULL);
        printf("przyjeto %d bajtow '%s'\n", d, buf);
        return 0;
    }
}







// FILE *ws;
// ws = popen("grep syla", "w");
// fputs("tekst przeslany jako ze stdin", ws);
// pclose(ws);
// 
// return 0;