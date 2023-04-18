#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(){
    int w1;
    mkfifo("potok", S_IFIFO | S_IRWXU);

    w1 = open("potok", O_WRONLY);
    
    write(w1, "1234567890", 11);


    unlink("potok");
    close(w1);
    return 0;
}
