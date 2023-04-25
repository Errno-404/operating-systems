#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <fcntl.h>

#include <unistd.h>
#include <sys/select.h>
#include "client_server.h"

// TODO naprawa czytania linii poleceń, dodanie warunków i obsługi błędów, przeniesienie do funkcji tych gigantów

int client_ipc_id, server_msgid;
MessageBuffer *message;


// For unblocking input, because sending STOP via IPC and doing the same via signal would be redundant
struct timeval tv;
fd_set readfds;
int retval;
int fd = 0;

void stop_client()
{
    message->mesg_type = STOP;
    msgsnd(server_msgid, message, sizeof(MessageBuffer), 0);

    msgctl(client_ipc_id, IPC_RMID, NULL);
    exit(0);
}






int main()
{

    key_t server_ipc_key = ftok(getenv("HOME"), PROJ_ID);
    server_msgid = msgget(server_ipc_key, 0666);

    // tworzymy własną kolejkę
    srand(time(NULL));
    key_t key = ftok(getenv("HOME"), rand() % 255 + 1);
    client_ipc_id = msgget(key, 0666 | IPC_CREAT);

    // wysyłamy message do servera ze swoim key  i czekamy na potwierdzenie
    message = malloc(sizeof(MessageBuffer));

    message->mesg_type = INIT;
    message->client_key = key;

    msgsnd(server_msgid, message, sizeof(MessageBuffer), 0);
    msgrcv(client_ipc_id, message, sizeof(MessageBuffer), INIT, 0);

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

    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
    while (1)
    {

        if (msgrcv(client_ipc_id, message, sizeof(MessageBuffer), 0, IPC_NOWAIT) != -1)
        {

            if (message->mesg_type == STOP)
                stop_client();

            if (message->mesg_type == TOALL)
            {
                printf("now: %d-%02d-%02d %02d:%02d:%02d\n", message->tm.tm_year + 1900, message->tm.tm_mon + 1, message->tm.tm_mday, message->tm.tm_hour, message->tm.tm_min, message->tm.tm_sec);
                fflush(stdout);
            }
            if(message->mesg_type == TOONE){
                printf("%d", message->dest);
                fflush(stdout);
            }
        }

        // testing input
        FD_ZERO(&readfds);
        FD_SET(fd, &readfds);
        tv.tv_sec = 0;
        tv.tv_usec = 0;

        retval = select(fd + 1, &readfds, NULL, NULL, &tv);
        if (retval == -1)
        {
            perror("select");
        }
        else if (retval)
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
                        message->mesg_type = TOONE;
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

    }

    msgctl(client_ipc_id, IPC_RMID, NULL);

    return 0;
}
