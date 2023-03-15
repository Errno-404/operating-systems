#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/times.h>

#define BUFF_SIZE 1
#define PERMISIONS 0664
#define ONEBYTE 1

int reverse_block(char *input_file, char *output_file)
{

    int fd_i = open(input_file, O_RDONLY);
    int fd_o = open(output_file, O_CREAT | O_WRONLY | O_TRUNC, PERMISIONS);

    if (fd_i == -1 || fd_o == -1)
    {
        printf("open() error!\n");
        return 1;
    }

    char buff[BUFF_SIZE];
    size_t nread;
    size_t written;

    lseek(fd_i, -1, SEEK_END);

    while ((nread = read(fd_i, buff, ONEBYTE)) > 0)
    {

        written = write(fd_o, buff, 1);
        if (written != nread)
        {
            printf("write() error!\n");
        }

        if ((lseek(fd_i, -2, SEEK_CUR)) < 0)
        {
            break;
        }
    }

    close(fd_i);
    close(fd_o);

    return 0;
}

static void pr_times(clock_t real, struct tms *tmsstart, struct tms *tmsend);

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Expected 2 arguments but got: %d instead!\n", argc);
        return 1;
    }

    char input_file[FILENAME_MAX];
    char output_file[FILENAME_MAX];

    strcpy(input_file, argv[1]);
    strcpy(output_file, argv[2]);

    struct stat buf;
    struct tms tmsstart, tmsend;
    clock_t start, end;

    if (access(input_file, F_OK) == -1)
    {
        printf("%s: There is no such file or directory!\n", input_file);
        return 1;
    }

    stat(input_file, &buf);

    if (S_ISDIR(buf.st_mode))
    {
        printf("%s is a directory!\n", input_file);
        return 1;
    }

    start = times(&tmsstart);
    int ret = reverse_block(input_file, output_file);
    end = times(&tmsend);

    pr_times(end - start, &tmsstart, &tmsend);
    return ret;
}

static void pr_times(clock_t real, struct tms *tmsstart, struct tms *tmsend)
{
    static long clktck = 0;
    if (clktck == 0)
    {
        clktck = sysconf(_SC_CLK_TCK);
    }

    fprintf(stdout, "==================\n");
    fprintf(stdout, " real: %10.8f\n", real / (double)clktck);
    fprintf(stdout, " user: %10.8f\n", (tmsend->tms_utime - tmsstart->tms_utime) / (double)clktck);
    fprintf(stdout, " sys: %11.8f\n", (tmsend->tms_stime - tmsstart->tms_stime) / (double)clktck);
    fprintf(stdout, "==================\n");
}