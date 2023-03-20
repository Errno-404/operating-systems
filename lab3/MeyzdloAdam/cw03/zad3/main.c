// poczytać Stevensa :>
// perror!

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/limits.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/wait.h>

#define STRING_MAX 255

int handle_directory(char *dir_path, char *some_string)
{

    // making struct for individual process

    struct stat buf;
    struct dirent *current_file;
    DIR *current_dir;

    // po co?
    char current_file_path[PATH_MAX];

    // opening current process directory
    if ((current_dir = opendir(dir_path)) == NULL)
    {
        printf("opendir error");
        return 1;
    }

    errno = 0;

    // reading all positions in directory
    while ((current_file = readdir(current_dir)) != NULL)
    {

        // creates path to currently checked file <- to be contiunued xD
        strcpy(current_file_path, dir_path);
        strcat(current_file_path, "/");
        strcat(current_file_path, current_file->d_name);

        if ((stat(current_file_path, &buf)) != 0)
        {
            printf("stat error");
            return 1;
        }

        // pomijamy foldery . i ..
        if (strcmp(current_file->d_name, ".") == 0 || strcmp(current_file->d_name, "..") == 0)
        {
            continue;
        }

        if (!S_ISDIR(buf.st_mode))
        {

            // zrobić funkcję z tego!

            if (!access(current_file_path, R_OK))
            {

                char buff[STRING_MAX];
                int fd = open(current_file_path, O_RDONLY);
                if (fd == -1)
                {
                    printf("open() returned -1!\n");
                    return 1;
                }

                int nread = read(fd, buff, STRING_MAX);
                if (nread == -1)
                {
                    printf("read() error!\n");
                    return 1;
                }

                close(fd);

                if (strncmp(buff, some_string, strlen(some_string)) == 0)
                {
                    // tutaj mamy już pewność że plik się właściwie zaczyna!
                    printf("path: %s pid: \n", current_file_path);
                    fflush(stdout);
                }
            }
        }
        else
        {

            int status;
            // tutaj wywołać proces?? bez forków działa rekurencyjnie po prostu

            int pid;
            if ((pid = fork()) == 0)
            {
                printf("dir path: %s", dir_path);
                printf("pid: %d, parent pid: %d folder: %s\n", getpid(), getppid(), current_file_path);
                fflush(stdout);
                

                if (handle_directory(current_file_path, some_string) == 1)
                {
                    return 1;
                }

            }
            else if (pid > 0){
                waitpid(pid, &status, WUNTRACED);
            }
            else
            {
                return 0;
            }
        }
    }

    closedir(current_dir);
    exit(0);
    if (errno != 0)
    {
        printf("readdir error");
        return 1;
    }
    // exit(0);
    return 0;
}

// ENDOF FUNCTIONS

int main(int argc, char *argv[])
{

    if (argc != 3)
    {
        printf("Expected 2 arguments, got %d instead!\n", argc - 1);
        return 1;
    }

    if (strlen(argv[1]) > PATH_MAX)
    {
        printf("The maximum path size is %d!\n", PATH_MAX);
        return 1;
    }

    if (strlen(argv[2]) > STRING_MAX)
    {
        printf("The maximum length of the second argument is %d!\n", STRING_MAX);
        return 1;
    }

    char dir_path[PATH_MAX];
    strcpy(dir_path, argv[1]);

    char some_string[255];
    strcpy(some_string, argv[2]);

    return handle_directory(dir_path, some_string);
}
