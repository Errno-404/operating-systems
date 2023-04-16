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
    char arr1[80];
    double result;

    mkfifo(FIFO_FILE, 0666);
    pid_t pid;
    for (int i = 0; i < num_processes; i++)
    {

        pid = fork();

        if (pid == -1)
        {
            printf("Failed to create sub process!");
            exit(1);
        }
        else if (pid == 0)
        {
            // Child process (sub.c)
            char interval[30];

            double start = i * 1.0 / num_processes;
            double end = (i + 1) * 1.0 / num_processes;

            snprintf(interval, 30, "%f-%f, %lf", start, end, dx);
            execl("./sub", "sub", interval, NULL);
        }
        else
        {
            char arr2[80];
            fd = open(FIFO_FILE, O_RDONLY);
            read(fd, arr2, sizeof(arr2));

            result += atof(arr2);

            close(fd);
        }
    }
    printf("Message received from main process: %lf\n", result);
    unlink("myfifo");
    return 0;
}
