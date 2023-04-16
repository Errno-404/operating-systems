#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>

#define FIFO_FILE "myfifo"

double f(double x)
{
    return 4.0 / (x * x + 1);
}

double integrate(double a, double b, double dx)
{
    double x, sum;
    sum = 0.0;
    for (x = a; x < b; x += dx)
    {
        sum += f(x) * dx;
    }

    return sum;
}

int main(int argc, char *argv[])
{
    char *interval_str;
    double a, b, dx, result;

    interval_str = argv[1];
    sscanf(interval_str, "%lf-%lf, %lf", &a, &b, &dx);

    result = integrate(a, b, dx);
    int fd;
    if ((fd = open(FIFO_FILE, O_WRONLY)) == -1)
    {
        perror("open() error");
        exit(1);
    }

    char arr1[16];
    sprintf(arr1, "%lf", result);

    if (write(fd, arr1, sizeof(arr1)) == -1)
    {
        perror("write() error");
        exit(1);
    }
    close(fd);

    return 0;
}
