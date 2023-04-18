#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/times.h>
#include <fcntl.h>

#define PIPE_NAME "integral_pipe"

double f(double x)
{
    return 4.0 / (x * x + 1);
}

void compute_integral(double start, double end, double dx, int pipefd)
{
    double result = 0.0;
    double x;
    for (x = start; x < end; x += dx)
    {
        result += f(x) * dx;
    }
    write(pipefd, &result, sizeof(result));
    close(pipefd); // close write end of the pipe
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        fprintf(stderr, "Usage: %s start end dx\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    double start = atof(argv[1]);
    double end = atof(argv[2]);
    double dx = atof(argv[3]);

    // open pipe for writing
    int pipefd = open(PIPE_NAME, O_WRONLY);
    if (pipefd == -1)
    {
        perror("open");
        exit(EXIT_FAILURE);
    }

    compute_integral(start, end, dx, pipefd);

    return 0;
}
