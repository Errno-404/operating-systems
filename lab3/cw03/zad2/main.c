#include <stdio.h>
#include <unistd.h>

int main(int argc, char * argv[]){
    if(argc != 2){
        printf("Expected 1 argument, got %d instead!\n", argc - 1);
        return 1;
    }

    printf("%s", argv[0]);
    fflush(stdout);
    execl("/bin/ls", "ls", argv[1], (char *) NULL);
}