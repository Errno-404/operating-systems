#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <ctype.h>
#include <sys/times.h>

#define FIFO_FILE "myfifo"

static void pr_times(FILE *fp, clock_t real, struct tms *tmsstart, struct tms *tmsend, double dx, int num_processes);

int main(int argc, char *argv[])
{
    // checking for the arguments
    if (argc < 3)
    {
        fprintf(stderr, "Expected 2 arguments, got %d instead\n", argc - 1);
        exit(EXIT_FAILURE);
    }

    int num_processes = atoi(argv[2]);
    double dx = atof(argv[1]);

    pid_t pid;
    int fd;
    double total = 0.0;

    // creating fifo
    mkfifo(FIFO_FILE, 0666);

    struct tms tmsstart, tmsend;
    clock_t start, end;

    start = times(&tmsstart);
    // making `num_processes` programs ran by execl
    for (int i = 0; i < num_processes; i++)
    {
        pid = fork();

        if (pid < 0)
        {
            perror("fork() error in main.c\n");
            exit(1);
        }
        else if (pid == 0)
        {

            double left = i * 1.0 / num_processes;
            double right = (i + 1) * 1.0 / num_processes;

            // preparing second argument
            char interval_str[256];
            sprintf(interval_str, "(%lf, %lf); %lf", left, right, dx);

            // executing sub-program
            execl("sub", "sub", FIFO_FILE, interval_str, NULL);
            perror("execlp() error in main.c\n");
            exit(1);
        }
        else
        {
            // reading fifo
            fd = open(FIFO_FILE, O_RDONLY);
            double sol;
            if(read(fd, &sol, sizeof(double)) == -1){
                perror("read() error in main.c!\n");
                exit(EXIT_FAILURE);
            }

            total += sol;
            close(fd);

            // waiting for the child process to end
            wait(NULL);
        }
    }
    end = times(&tmsend);
    // removing fifo file
    unlink(FIFO_FILE);

    printf("Integral value: %lf\n", total);
    pr_times(stdout, end - start, &tmsstart, &tmsend, dx, num_processes);


    FILE *fp;
    fp = fopen("raport.txt", "a");
    if (fp == NULL)
    {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    pr_times(fp, end - start, &tmsstart, &tmsend, dx, num_processes);
    fclose(fp);

    return 0;
}

static void pr_times(FILE *fp, clock_t real, struct tms *tmsstart, struct tms *tmsend, double dx, int num_processes)
{
    static long clktck = 0;
    if (clktck == 0)
    {
        clktck = sysconf(_SC_CLK_TCK);
    }

    fprintf(fp, "Execution time with %d processes and dx=%lf: %f seconds\n", num_processes, dx, real / (double)clktck);
}
