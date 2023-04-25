#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <signal.h>
#include "client_server.h"
#define MAX 10

int new_client_id = 0;

// TODO clien_id managing

int clients[10];
int msgid;

void list_all()
{
    printf("Active clients:\n");
    for (int i = 0; i < 10; i++)
    {
        if (clients[i] != -1)
        {
            printf(" * ID: %d\n", clients[i]);
        }
    }
    fflush(stdout);
}

void close_server()
{
    msgctl(msgid, IPC_RMID, NULL);
    exit(0);
}

int main()
{
    // tworzymy kolejkę
    key_t key = ftok(getenv("HOME"), PROJ_ID);
    msgid = msgget(key, 0666 | IPC_CREAT);

    // tworzymy strukturę na komunikaty
    MessageBuffer *message = malloc(sizeof(MessageBuffer));

    for (int i = 0; i < 10; i++)
    {
        clients[i] = -1;
    }

    signal(SIGINT, close_server);

    // główna pętla w której oczekujemy na komunikaty od klientów
    while (1)
    {
        msgrcv(msgid, message, sizeof(MessageBuffer), PRIORITY, 0);

        switch (message->mesg_type)
        {
        case INIT:
            key_t client_key = message->client_key;
            int client_msgid = msgget(client_key, 0666);

            message->client_id = new_client_id;

            if (new_client_id < MAX_NO_CLIENTS)
            {
                clients[new_client_id] = new_client_id;

                // update new_client_id
                new_client_id++;
            }

            msgsnd(client_msgid, message, sizeof(MessageBuffer), 0);
            break;

        case LIST:
            list_all();
            break;

        case TOALL:
            printf("%s", message->message);
            printf("now: %d-%02d-%02d %02d:%02d:%02d\n", message->tm.tm_year + 1900, message->tm.tm_mon + 1, message->tm.tm_mday, message->tm.tm_hour, message->tm.tm_min, message->tm.tm_sec);
            fflush(stdout);
            break;

        case TOONE:
            printf("%d", message->dest);
            fflush(stdout);
            break;
        case STOP:
            printf("%d", message->client_id);
            fflush(stdout);

            int this_client_id = message->client_id;
            clients[this_client_id] = -1;
            list_all();

            if (this_client_id < new_client_id)
            {
                new_client_id = this_client_id;
            }

            break;

        default:
            break;
        }
    }

    msgctl(msgid, IPC_RMID, NULL);

    return 0;
}