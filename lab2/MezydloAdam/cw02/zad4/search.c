#include <ftw.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

long long total = 0;

int visit(const char *fpath, const struct stat *sb, int typeflag);

/* Main start */
int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        printf("You have to pass at least one argument!\n");
        return 1;
    }
    else if (argc > 2)
    {
        printf("Too many arguments!\n");
        return 1;
    }

    char filepath[FILENAME_MAX];
    strcpy(filepath, argv[1]);

    ftw(filepath, &visit, 30);
    printf("%5lld total\n", total);
}
/* Main end */

int visit(const char *fpath, const struct stat *sb, int typeflag)
{
    if (!S_ISDIR(sb->st_mode))
    {

        printf("%5ld", sb->st_size);
        total += sb->st_size;

        printf(" %s\n", fpath);
    }

    return 0;
}
