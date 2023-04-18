#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

double f(double x)
{
    return 4.0 / (x * x + 1);
}

double integrate(double a, double b, double dx)
{
    double sum = 0.0;
    for (double x = a; x < b; x += dx)
    {
        sum += (f(x) * dx);
    }

   

    return sum;
}

int main(int argc, char *argv[])
{
    // getting (a, b); dx
    double a, b, dx;
    sscanf(argv[2], "(%lf, %lf); %lf", &a, &b, &dx);

    int fifo_fd;

    // Open the FIFO queue for writing
    if((fifo_fd = open(argv[1], O_WRONLY)) == -1){

        perror("open() error in sub.c!\n");
        exit(EXIT_FAILURE);
    }

    double result = integrate(a, b ,dx);

    if(write(fifo_fd, &result, sizeof(result)) == -1){
        perror("write() error in sub.c");
    }

    // Close the FIFO queue
    close(fifo_fd);

    return 0;
}
