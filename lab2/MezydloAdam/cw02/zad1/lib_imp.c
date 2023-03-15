#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/times.h>

#define BUFF_SIZE 1024

int sys_imp(char find_c, char replace_with_c, char *input_file, char *output_file)
{

    FILE *fp_i = fopen(input_file, "r");
    FILE *fp_o = fopen(output_file, "w");

    if (fp_i == NULL || fp_o == NULL)
    {
        printf("fopen() error!\n");
        return 1;
    }

    char buff[BUFF_SIZE];

    size_t nread;
    size_t written;

    while ((ferror(fp_i)) == 0 && (feof(fp_i)) == 0)
    {
        nread = fread(buff, sizeof(char), sizeof(buff), fp_i);

        for (size_t i = 0; i < nread; ++i)
        {
            if (buff[i] == find_c)
            {
                buff[i] = replace_with_c;
            }
        }

        if (ferror(fp_i))
        {
            printf("fread() error!\n");
            return 1;
        }

        written = fwrite(buff, sizeof(char), nread, fp_o);
        if (written != nread)
        {
            printf("Error while reading and writing files!\n");
            return 1;
        }
    }

    if ((ferror(fp_i)) != 0)
    {
        printf("fread() or fwrite() error!\n");
        return 1;
    }

    fclose(fp_i);
    fclose(fp_o);

    return 0;
}

static void pr_times(clock_t real, struct tms *tmsstart, struct tms *tmsend);

int main(int argc, char *argv[])
{
    if (argc != 5)
    {
        printf("Expected 4 arguments but got: %d instead!\n", argc);
        return 1;
    }
    else if (strlen(argv[1]) != 1)
    {
        printf("\"%s\" is not an individual character!\nExpected character but got string instead\n", argv[1]);

        return 1;
    }
    else if (strlen(argv[2]) != 1)
    {
        printf("\"%s\" is not an individual character!\nExpected character but got string instead!\n", argv[2]);
        return 1;
    }

    char find_c = argv[1][0];
    char replace_with_c = argv[2][0];
    char input_file[FILENAME_MAX];
    char output_file[FILENAME_MAX];

    strcpy(input_file, argv[3]);
    strcpy(output_file, argv[4]);

    struct stat buf;
    struct tms tmsstart, tmsend;
    clock_t start, end;

    if (access(input_file, F_OK) == -1)
    {
        printf("There is no such file or directory!\n");
        return 1;
    }

    stat(input_file, &buf);

    if (S_ISDIR(buf.st_mode))
    {
        printf("%s is a directory!\n", input_file);
        return 1;
    }

    start = times(&tmsstart);
    int ret = sys_imp(find_c, replace_with_c, input_file, output_file);
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