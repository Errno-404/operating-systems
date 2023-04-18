#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/times.h>
#include <fcntl.h>
#include <string.h>

#define PIPE_NAME "integral_pipe"

double f(double x)
{
    return 4.0 / (x * x + 1);
}

static void pr_times(FILE *, clock_t, struct tms *, struct tms *, double, int);

void compute_integral(double start, double end, double dx, FILE *pipe)
{
    double result = 0.0;
    double x;
    for (x = start; x < end; x += dx)
    {
        result += f(x) * dx;
    }
    fprintf(pipe, "%lf\n", result);
    fflush(pipe); // flush the output buffer
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
    pid_t pid[num_processes];
    double total = 0.0;

    struct tms tmsstart, tmsend;
    clock_t start, end;

    start = times(&tmsstart);

    // create named pipe
    if (mkfifo(PIPE_NAME, 0666) == -1)
    {
        perror("mkfifo");
        exit(EXIT_FAILURE);
    }

    // create child processes
    for (i = 0; i < num_processes; i++)
    {
        pid[i] = fork();
        if (pid[i] == -1)
        {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        else if (pid[i] == 0)
        {
            // open pipe for writing
            FILE *pipe = popen("./integral_subprocess", "w");
            if (pipe == NULL)
            {
                perror("popen");
                exit(EXIT_FAILURE);
            }

            double start = i * 1.0 / num_processes;
            double end = (i + 1) * 1.0 / num_processes;
            compute_integral(start, end, dx, pipe);

            pclose(pipe);
            exit(EXIT_SUCCESS);
        }
    }

    // open pipe for reading
    int pipefd = open(PIPE_NAME, O_RDONLY);
    if (pipefd == -1)
    {
        perror("open");
        exit(EXIT_FAILURE);
    }

    // read results from child processes
    char buffer[1024];
    for (i = 0; i < num_processes; i++)
    {
        double result;
        fgets(buffer, sizeof(buffer), stdin);
        result = atof(buffer);
        total += result;
    }

    // close pipe
    close(pipefd);

    // wait for child processes to finish
    for (i = 0; i < num_processes; i++)
    {
        waitpid(pid[i], NULL, 0);
    }

    end = times(&tmsend);

    printf("Integral value: %lf\n", total);
    // printf("Execution time with %d processes
