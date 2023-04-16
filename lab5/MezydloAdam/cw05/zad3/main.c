// szkielet fifo z exec

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>

#define FIFO_FILE "myfifo"

int main(int argc, char *argv[])
{

    if (argc < 3)
    {
        fprintf(stderr, "Usage: %s dx num_processes\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    double dx = atof(argv[1]);
    int num_processes = atoi(argv[2]);

    int fd;
    double result = 0.0;

    if(mkfifo(FIFO_FILE, 0666) == -1){
        perror("mkfifo() error");
        exit(1);
    }

    pid_t pid;
    for (int i = 0; i < num_processes; i++)
    {

        pid = fork();

        if (pid == -1)
        {
            printf("fork() error");
            exit(1);
        }
        else if (pid == 0)
        {   
            double start = i * 1.0 / num_processes;
            double end = (i + 1) * 1.0 / num_processes;

            char interval[30];
            snprintf(interval, 30, "%f-%f, %lf", start, end, dx);
            if(execl("./sub", "sub", interval, NULL) == -1){
                perror("execl() error");
                exit(1);
            }
        }
        else
        {
            char returned[16];
            if((fd = open(FIFO_FILE, O_RDONLY)) == -1){
                perror("open() error");
                exit(1);
            }
            if(read(fd, returned, sizeof(returned)) == -1){
                perror("read() error");
                exit(1);
            }

            result += atof(returned);
            close(fd);
        }
    }
    printf("Message received from main process: %lf\n", result);
    unlink("myfifo");
    return 0;
}
