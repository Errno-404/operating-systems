#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

int w1;
char buf[1024];
int main(){
   w1 = open("potok", O_RDONLY);
   buf[w1] = 0;
   read(w1, buf, 11);
   printf("otrzymano: %s\n", buf);
   
   
   unlink("potok");
   close(w1);
    return 0;
}
