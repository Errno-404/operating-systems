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

int handle_directory(char *dir_path, char *some_string);

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

int handle_directory(char *dir_path, char *some_string)
{

    // making struct for individual process
    struct stat buf;
    struct dirent *current_file;
    DIR *current_dir;

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

        // creates path to currently checked file
        strcpy(current_file_path, dir_path);
        strcat(current_file_path, "/");
        strcat(current_file_path, current_file->d_name);

        if ((stat(current_file_path, &buf)) != 0)
        {
            printf("stat error");
            return 1;
        }

        // omit . and ..
        if (strcmp(current_file->d_name, ".") == 0 || strcmp(current_file->d_name, "..") == 0)
        {
            continue;
        }

        // when file occured
        if (!S_ISDIR(buf.st_mode))
        {
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

                // checking the beggining of the file
                if (strncmp(buff, some_string, strlen(some_string)) == 0)
                {
                    char tmp[PATH_MAX];
                    if(realpath(current_file_path, tmp) == NULL){
                        strcpy(tmp, current_file_path);
                    }
                    printf("path: %s pid: %d\n", tmp, getpid());
                    fflush(stdout);
                }
            }
        }
        else
        {
            // creating new process if file is actually a directory
            int status, pid;
            if ((pid = fork()) == 0)
            {
                if (handle_directory(current_file_path, some_string) == 1)
                {
                    printf("handle_directory function returned 1!\n");
                    return 1;
                }
            }
            else if (pid > 0)
            {
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
        printf("readdir error!\n");
        return 1;
    }

    return 0;
}