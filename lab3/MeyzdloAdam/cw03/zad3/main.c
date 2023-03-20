// poczytać Stevensa :>

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


#define STRING_MAX 255


int handle_directory(char * dir_path, char * some_string)
{
    

    // making struct for individual process

    char current_file_path[PATH_MAX];

    DIR *current_dir;
    struct dirent *current_file;
    struct stat buf;
    
    
    

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

        if (!S_ISDIR(buf.st_mode))
        {
            printf("%s\n", current_file->d_name);
            
        }
        else{
            printf("%d", strcmp(current_file->d_name))
            printf("%s", current_file->d_name);
        }
        
         
    }

    closedir(current_dir);
    if (errno != 0)
    {
        printf("readdir error");
        return 1;
    }
}

// ENDOF FUNCTIONS



int main(int argc, char *argv[])
{

    if (argc != 3)
    {
        printf("Expected 2 arguments, got %d instead!\n", argc - 1);
        return 1;
    }

    if(strlen(argv[1]) > PATH_MAX){
        printf("The maximum path size is %d!\n", PATH_MAX);
        return 1;
    }

    if(strlen(argv[2]) > STRING_MAX){
        printf("The maximum length of the second argument is %d!\n", STRING_MAX);
        return 1;
    }

    char dir_path[PATH_MAX];
    strcpy(dir_path, argv[1]);

    char some_string[255];
    strcpy(some_string, argv[2]);


    return handle_directory(dir_path, some_string);
   
}
