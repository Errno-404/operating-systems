#include "client_server.h"

int new_client_id = 0;

key_t server_ipc_key;
int server_ipc_id;

MessageBuffer *message;
int active_clients[MAX_NO_CLIENTS];

int client_msgids[MAX_NO_CLIENTS];

void initialize_server();
void init_client_connection();
void finish_client_work();

void display_active_clients();
void stop_server();
void update_new_client_id();

void send_to_all(MessageBuffer *);
void send_to_one(MessageBuffer *);

void save_log(int, int, struct tm);

struct tm get_time();
struct tm tm;

int main()
{

    initialize_server();

    while (1)
    {
        msgrcv(server_ipc_id, message, MSG_SIZE, PRIORITY, 0);

        switch (message->mesg_type)
        {
        case INIT:
            init_client_connection();
            break;

        case LIST:
            tm = get_time();
            display_active_clients();
            save_log(message->mesg_type, message->client_id, tm);
            break;

        case TOALL:
            tm = get_time();
            send_to_all(message);
            save_log(message->mesg_type, message->client_id, tm);
            break;

        case TOONE:
            tm = get_time();
            send_to_one(message);
            save_log(message->mesg_type, message->client_id, tm);
            break;
        case STOP:
           tm = get_time();
            finish_client_work();
            save_log(message->mesg_type, message->client_id, tm);
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
    server_ipc_key = ftok(getenv("HOME"), PROJ_ID);
    server_ipc_id = msgget(server_ipc_key, 0666 | IPC_CREAT);

    message = malloc(MSG_SIZE);

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

    msgsnd(client_msgid, message, MSG_SIZE, 0);
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
            msgsnd(client_msgids[i], message, MSG_SIZE, 0);
            msgrcv(server_ipc_id, message, MSG_SIZE, STOP, 0);
        }
    }

    free(message);
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
    MessageBuffer *tosend = malloc(MSG_SIZE);
    int this_client_id = message->client_id;

    tosend->client_id = this_client_id;
    tosend->mesg_type = TOALL;
    strcpy(tosend->message, message->message);

    tosend->tm = get_time();

    for (int i = 0; i < MAX_NO_CLIENTS; i++)
    {
        if (client_msgids[i] != -1 && client_msgids[i] != client_msgids[this_client_id])
        {
            msgsnd(client_msgids[i], tosend, MSG_SIZE, 0);
        }
    }
    free(tosend);
}

struct tm get_time()
{
    time_t t = time(NULL);
    return *localtime(&t);
}

void save_log(int type, int id, struct tm tm)
{
    FILE *fp = fopen("log.txt", "a");


  
    fprintf(fp, "Type: %d\nClient ID: %d\nServer  time: %d-%02d-%02d %02d:%02d:%02d\n", type, id, tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min,tm.tm_sec);

    fclose(fp);
}

void send_to_one(MessageBuffer *message)
{
    int dest = message->dest;

    MessageBuffer *tosend = malloc(MSG_SIZE);
    tosend->client_id = message->client_id;
    tosend->mesg_type = TOONE;
    strcpy(tosend->message, message->message);
    tosend->dest = dest;

    tosend->tm = get_time();

    if (dest >= 0 && dest < MAX_NO_CLIENTS && client_msgids[dest] != -1)
    {
        msgsnd(client_msgids[dest], tosend, MSG_SIZE, 0);
    }
    free(tosend);
}