#include <stdio.h>
#include <unistd.h>

int main(){

    pid_t pid;
    int fd[2];
    int w;
    char buf[256];

    pipe(fd);
    pid = fork();
    if(pid == 0){
        close(fd[0]);
        w = write(fd[1], "1234567890", 10);
        // sleep(10);
        return 0;
    }
    close(fd[1]);
    w = read(fd[0], buf, 10);
    buf[w] = 0;
    printf("%s\n", buf);fflush(stdout);

}