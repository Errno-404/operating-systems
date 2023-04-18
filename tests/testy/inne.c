#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct pakiet
{
    long typ; // musi byc long
    int zawartosc; // dowolne, ale rozmiar <= msgmax
};

int main(void)
{
    struct pakiet o1;
    key_t klucz = ftok("./plik2", 'p');
    int id_kolejki_kom = msgget(klucz, 0600);

    // odbior

    while (1)
    {
        if (msgrcv(id_kolejki_kom, &o1, sizeof(struct pakiet), 1, IPC_NOWAIT) < 0)
            break;

        printf("otrzymano o1.zawartosc");
    }

    return 0;
}