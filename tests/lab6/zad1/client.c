#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include "client_server.h"

// TODO naprawa czytania linii poleceń, dodanie warunków i obsługi błędów, przeniesienie do funkcji tych gigantów

int msgid, server_msgid;
MessageBuffer *message;

void stop_client()
{
    message->mesg_type = STOP;
    msgsnd(server_msgid, message, sizeof(MessageBuffer), 0);

    msgctl(msgid, IPC_RMID, NULL);
    exit(0);
}

int main()
{

    // po uruchomieniu klienta pobieramy kolejkę serwera
    key_t server_key = ftok(getenv("HOME"), PROJ_ID);
    server_msgid = msgget(server_key, 0666);

    // tworzymy własną kolejkę
    srand(time(NULL));
    key_t key = ftok(getenv("HOME"), rand() % 255 + 1);
    msgid = msgget(key, 0666 | IPC_CREAT);

    // wysyłamy message do servera ze swoim key  i czekamy na potwierdzenie
    message = malloc(sizeof(MessageBuffer));

    message->mesg_type = INIT;
    message->client_key = key;

    msgsnd(server_msgid, message, sizeof(MessageBuffer), 0);
    msgrcv(msgid, message, sizeof(MessageBuffer), INIT, 0);

    // if server overload then exit
    if (message->client_id == MAX_NO_CLIENTS)
    {
        printf("Server overloaded!\n");
        stop_client();
    }

    signal(SIGINT, stop_client);

    // parsing input

    char line[1024];
    char cmd[32];
    char string[1024];
    int id;
    while (1)
    {
        fgets(line, sizeof(line), stdin);
        if (strcmp(line, "LIST\n") == 0)
        {
            message->mesg_type = LIST;
            msgsnd(server_msgid, message, sizeof(MessageBuffer), 0);
        }
        else if (strcmp(line, "STOP\n") == 0)
        {
            message->mesg_type = STOP;
            msgsnd(server_msgid, message, sizeof(MessageBuffer), 0);
            break;
        }
        else
        {
            if (sscanf(line, "%s %d %s", cmd, &id, string) == 3)
            {
                if (strcmp(cmd, "2ONE") == 0)
                {
                    message->mesg_type = TOALL;
                    strcpy(message->message, string);
                    time_t t = time(NULL);
                    message->tm = *localtime(&t);
                    message->dest = id;
                    msgsnd(server_msgid, message, sizeof(MessageBuffer), 0);
                }
            }
            else if (sscanf(line, "%s %s", cmd, string) == 2)
            {
                if (strcmp(cmd, "2ALL") == 0)
                {
                    message->mesg_type = TOALL;
                    strcpy(message->message, string);
                    time_t t = time(NULL);
                    message->tm = *localtime(&t);
                    msgsnd(server_msgid, message, sizeof(MessageBuffer), 0);
                }
            }
        }

        line[0] = '\0';
    }

    msgctl(msgid, IPC_RMID, NULL);

    return 0;
}