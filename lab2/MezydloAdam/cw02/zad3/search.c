#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>

#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

int main()
{

    // potrzebne struktury związane z folderami

    struct stat buf;
    DIR *current_dir;
    struct dirent *current_file;

    // getting path like /home/adam/~
    char current_dir_path[FILENAME_MAX];
    if ((getcwd(current_dir_path, FILENAME_MAX)) == NULL)
    {
        printf("getcwd error");
        return 1;
    }

    // will be changed in every loop iteration
    char current_file_path[FILENAME_MAX];

    // opens current directory
    if ((current_dir = opendir(current_dir_path)) == NULL)
    {
        printf("opendir error");
        return 1;
    }

    errno = 0;
    long long total = 0;
    while ((current_file = readdir(current_dir)) != NULL)
    {

        // creates path to currently checked file
        strcpy(current_file_path, current_dir_path);
        strcat(current_file_path, "/");
        strcat(current_file_path, current_file->d_name);

        if ((stat(current_file_path, &buf)) != 0)
        {
            printf("stat error");
            return 1;
        }

        if (!S_ISDIR(buf.st_mode))
        {
            printf("%5ld %s\n", buf.st_size, current_file->d_name);
            total += buf.st_size;
        }
    }
    closedir(current_dir);
    if (errno != 0)
    {
        printf("readdir error");
        return 1;
    }

    printf("%5lld total\n", total);

    return 0;
}