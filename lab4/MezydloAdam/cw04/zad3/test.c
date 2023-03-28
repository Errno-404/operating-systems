#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/times.h>

void fun(){
    sleep(1);
}

clock_t start, end;
int main()
{
    struct tms tmsstart, tmsend;

    start = times(&tmsstart);

    sleep(5);


    end = times(&tmsend);

    long clktck = sysconf(_SC_CLK_TCK);

    printf("%ld", (end - start)/ clktck);

}