#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include "client_server.h"

int new_client_id = 0;

key_t server_ipc_key;
int server_ipc_id;

MessageBuffer *message;
int active_clients[10];

int client_msgids[10];

void initialize_server();
void init_client_connection();
void finish_client_work();

void display_active_clients();
void stop_server();
void update_new_client_id();

void send_to_all(MessageBuffer *);
void send_to_one(MessageBuffer *);

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
            display_active_clients();
            break;

        case LIST:
            display_active_clients();
            break;

        case TOALL:
            send_to_all(message);

            break;

        case TOONE:
            send_to_one(message);
            break;
        case STOP:
            finish_client_work();
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

    for (int i = 0; i < MAX_NO_CLIENTS; i++)
    {
        active_clients[i] = -1;
    }

    for (int i = 0; i < MAX_NO_CLIENTS; i++)
    {
        client_msgids[i] = -1;
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
        client_msgids[new_client_id] = client_msgid;
        update_new_client_id();
    }

    msgsnd(client_msgid, message, sizeof(MessageBuffer), 0);
}

void display_active_clients()
{
    printf("Active clients:\n");
    for (int i = 0; i < MAX_NO_CLIENTS; i++)
    {
        if (active_clients[i] != -1)
        {
            printf(" * ID: %d\n", active_clients[i]);
        }
    }
    fflush(stdout);
}

void finish_client_work()
{
    int this_client_id = message->client_id;
    active_clients[this_client_id] = -1;
    client_msgids[this_client_id] = -1;

    // new_client_id update (always the lowest possible)
    if (this_client_id < new_client_id)
    {
        new_client_id = this_client_id;
    }
}

void stop_server()
{
    message->mesg_type = STOP;
    for (int i = 0; i < MAX_NO_CLIENTS; i++)
    {
        if (client_msgids[i] != -1)
        {
            msgsnd(client_msgids[i], message, sizeof(MessageBuffer), 0);
            msgrcv(server_ipc_id, message, sizeof(MessageBuffer), STOP, 0);
        }
    }

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

void send_to_all(MessageBuffer *message)
{
    MessageBuffer *tosend = malloc(sizeof(MessageBuffer));
    int this_client_id = message->client_id;

    tosend->client_id = this_client_id;
    tosend->mesg_type = TOALL;
    strcpy(tosend->message, message->message);
    tosend->tm = message->tm;

    for (int i = 0; i < MAX_NO_CLIENTS; i++)
    {
        if (client_msgids[i] != -1 && client_msgids[i] != client_msgids[this_client_id])
        {
            msgsnd(client_msgids[i], tosend, sizeof(MessageBuffer), 0);
        }
    }
}

void send_to_one(MessageBuffer *message)
{
    int dest = message->dest;

    MessageBuffer *tosend = malloc(sizeof(MessageBuffer));
    tosend->client_id = message->client_id;
    tosend->mesg_type = TOONE;
    strcpy(tosend->message, message->message);
    tosend->dest = dest;
    tosend->tm = message->tm;

    
    if (dest >= 0 && dest < MAX_NO_CLIENTS && client_msgids[dest] != -1)
    {
        msgsnd(client_msgids[dest], tosend, sizeof(MessageBuffer), 0);

    }

}