// wzór jak to zrobić: ale jeszcze trzeba obliczanie całki poprawić

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

double integrate(double a, double b) {
    double x, dx, sum;
    int n;

    n = 10000;
    dx = (b - a) / n;
    sum = 0;
    for (x = a + dx / 2; x < b; x += dx) {
        sum += x * x * dx;
    }

    return sum;
}

int main(int argc, char *argv[]) {
    char *interval_str;
    double a, b, result;
    int fd;

    // Parse interval from command line argument
    interval_str = argv[1];
    sscanf(interval_str, "%lf-%lf", &a, &b);

    // Calculate integral on interval
    result = integrate(a, b);

    // Open named pipe for writing
    fd = open("pipe", O_WRONLY);

    // Write result to pipe
    write(fd, &result, sizeof(double));

    // Close pipe and exit
    close(fd);
    exit(0);
}
