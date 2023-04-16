// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <math.h>
// #include <sys/wait.h>
// #include <sys/times.h>

// double f(double x)
// {
//     return 4.0 / (x * x + 1);
// }

// int main(int argc, char *argv[])
// {
//     if (argc != 3)
//     {
//         printf("Usage: %s dx num_processes\n", argv[0]);
//         return 1;
//     }

//     double dx = atof(argv[1]);
//     int num_processes = atoi(argv[2]);

//     int i, pid, status;
//     double integral = 0;
//     char command[128];
//     FILE *fp;
//     struct tms t_start, t_end;

//     clock_t clock_start, clock_end;
//     clock_start = times(&t_start);

//     for (i = 0; i < num_processes; i++)
//     {
//         double start = i * 1.0 / num_processes;
//         double end = (i + 1) * 1.0 / num_processes;

//         // generate command to run child program
//         sprintf(command, "./child %f %f %f", start, end, dx);

//         // open pipe to child process
//         fp = popen(command, "r");

//         if (fp == NULL)
//         {
//             printf("Error: failed to open pipe to child process\n");
//             return 1;
//         }

//         // read result from child process
//         double result;
//         fscanf(fp, "%lf", &result);

//         // add to total integral
//         integral += result;

//         // close pipe to child process
//         pclose(fp);
//     }

//     clock_end = times(&t_end);
//     double elapsed_time = (double)(clock_end - clock_start) / sysconf(_SC_CLK_TCK);

//     // print result and timing information
//     printf("Numerical integral of 4/(x^2+1) from 0 to 1: %f\n", integral);
//     printf("Elapsed time: %f seconds\n", elapsed_time);

//     return 0;
// }


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define NUM_PROCESSES 10
#define INTERVAL_SIZE 0.1

int main() {
    int fd;
    pid_t pids[NUM_PROCESSES];
    double results[NUM_PROCESSES];
    double total_result = 0;
    int i;

    // Create named pipe
    mkfifo("pipe", 0666);

    // Fork child processes to calculate intervals
    for (i = 0; i < NUM_PROCESSES; i++) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork failed");
            exit(1);
        } else if (pid == 0) {
            char interval[20];
            snprintf(interval, 20, "%.1f-%.1f", i * INTERVAL_SIZE, (i + 1) * INTERVAL_SIZE);
            execl("./child", "child", interval, NULL);
            exit(0);
        } else {
            pids[i] = pid;
        }
    }

    // Open named pipe for reading
    fd = open("pipe", O_RDONLY);

    // Read results from pipe
    for (i = 0; i < NUM_PROCESSES; i++) {
        read(fd, &results[i], sizeof(double));
    }

    // Close pipe
    close(fd);
    unlink("pipe");

    // Add up results
    for (i = 0; i < NUM_PROCESSES; i++) {
        total_result += results[i];
    }

    // Print total result
    printf("Total result: %f\n", total_result);

    return 0;
}
