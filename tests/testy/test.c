#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


struct pakiet{
    long typ;
    int zawartosc;
};


int main(void)
{
    int i;
    struct pakiet o1;

    struct pakiet o1;

    key_t klucz = ftok("./plik2", 'p');

        int id_kolejki_kom = msgget(klucz, IPC_CREAT | 0600);

    for (i = 0; i < 5; i++)
    {

        o1.typ = (i % 2) + 1;
        o1.zawartosc = i;
        msgsnd(id_kolejki_kom, &o1, sizeof(struct pakiet), 0);
    }
    return 0;
}