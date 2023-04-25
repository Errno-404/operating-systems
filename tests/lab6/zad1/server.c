#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include "client_server.h"
#define MAX 10


int client_id = 0;


int main()
{
    // tworzymy kolejkę
    key_t key = ftok(getenv("HOME"), PROJ_ID);
    int msgid = msgget(key, 0666 | IPC_CREAT);



    // tworzymy strukturę na komunikaty
    MessageBuffer *message = malloc(sizeof(MessageBuffer));

    // główna pętla w której oczekujemy na komunikaty od klientów
    while (1)
    {
        msgrcv(msgid, message, sizeof(MessageBuffer), PRIORITY, 0);

        switch (message->mesg_type)
        {
        case INIT:
            key_t client_key = message->client_key;
            int client_msgid = msgget(client_key, 0666);

            // na razie hardcoded client_id
            message->client_id = client_id;
            client_id++;
            msgsnd(client_msgid, message, sizeof(MessageBuffer), 0);
            break;

        case LIST:
            printf("%ld", message->mesg_type);
            fflush(stdout);
            break;

        default:
            break;
        }
    }

   



    msgctl(msgid, IPC_RMID, NULL);

    return 0;
}