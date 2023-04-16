#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/times.h>

double f(double x)
{
    return 4.0 / (x * x + 1);
}

static void pr_times(FILE *, clock_t, struct tms *, struct tms *, double, int);

void compute_integral(int pipefd[2], double start, double end, double dx)
{
    close(pipefd[0]); // close read end of the pipe
    double result = 0.0;
    double x;
    for (x = start; x < end; x += dx)
    {
        result += f(x) * dx;
    }
    write(pipefd[1], &result, sizeof(result));
    close(pipefd[1]); // close write end of the pipe
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        fprintf(stderr, "Usage: %s dx num_processes\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    double dx = atof(argv[1]);
    int num_processes = atoi(argv[2]);
    int i;
    pid_t pid;
    double total = 0.0;

    struct tms tmsstart, tmsend;
    clock_t start, end;

    int pipefd[num_processes][2]; // array of pipes

    start = times(&tmsstart);
    for (i = 0; i < num_processes; i++)
    {
        if (pipe(pipefd[i]) == -1)
        {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
        pid = fork();
        if (pid == -1)
        {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0)
        {
            double start = i * 1.0 / num_processes;
            double end = (i + 1) * 1.0 / num_processes;
            compute_integral(pipefd[i], start, end, dx);
        }
    }

    for (i = 0; i < num_processes; i++)
    {
        close(pipefd[i][1]); // close write end of the pipe
        double result;
        read(pipefd[i][0], &result, sizeof(result));
        close(pipefd[i][0]); // close read end of the pipe
        total += result;
    }

    for (i = 0; i < num_processes; i++)
    {
        wait(NULL); // wait for all child processes to finish
    }

    end = times(&tmsend);

    printf("Integral value: %lf\n", total);
    // printf("Execution time with %d processes and dx=%lf: %lf seconds\n", num_processes, dx, (end_time - start_time) / CLOCKS_PER_SEC);
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