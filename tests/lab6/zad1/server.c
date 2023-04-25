#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <signal.h>
#include "client_server.h"

int new_client_id = 0;

key_t server_ipc_key;
int server_ipc_id;

MessageBuffer *message;
int active_clients[10];

void initialize_server();
void init_client_connection();

void display_active_clients();
void stop_server();
void update_new_client_id();

int main()
{

    initialize_server();

    while (1)
    {
        msgrcv(server_ipc_id, message, sizeof(MessageBuffer), PRIORITY, 0);

        switch (message->mesg_type)
        {
        case INIT:
            init_client_connection();
            break;

        case LIST:
            display_active_clients();
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
            int this_client_id = message->client_id;
            active_clients[this_client_id] = -1;

            // new_client_id update (always the lowest possible)
            if (this_client_id < new_client_id)
            {
                new_client_id = this_client_id;
            }

            break;

        default:
            break;
        }
    }

    msgctl(server_ipc_id, IPC_RMID, NULL);

    return 0;
}

void initialize_server()
{
    // tworzymy kolejkę
    server_ipc_key = ftok(getenv("HOME"), PROJ_ID);
    server_ipc_id = msgget(server_ipc_key, 0666 | IPC_CREAT);

    // tworzymy strukturę na komunikaty
    message = malloc(sizeof(MessageBuffer));

    for (int i = 0; i < 10; i++)
    {
        active_clients[i] = -1;
    }

    signal(SIGINT, stop_server);
}

void init_client_connection()
{
    key_t client_key = message->client_key;
    int client_msgid = msgget(client_key, 0666);

    message->client_id = new_client_id;

    if (new_client_id < MAX_NO_CLIENTS)
    {
        active_clients[new_client_id] = new_client_id;
        update_new_client_id();
    }

    msgsnd(client_msgid, message, sizeof(MessageBuffer), 0);
}

void display_active_clients()
{
    printf("Active clients:\n");
    for (int i = 0; i < 10; i++)
    {
        if (active_clients[i] != -1)
        {
            printf(" * ID: %d\n", active_clients[i]);
        }
    }
    fflush(stdout);
}

void stop_server()
{
    msgctl(server_ipc_id, IPC_RMID, NULL);
    exit(0);
}

void update_new_client_id()
{
    int i = new_client_id + 1;
    while (i < MAX_NO_CLIENTS)
    {
        if (active_clients[i] == -1)
        {
            new_client_id = i;
            return;
        }
        i++;
    }
    new_client_id = i;
}