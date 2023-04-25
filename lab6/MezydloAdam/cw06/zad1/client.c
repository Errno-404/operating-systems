// Client could be better...



#include "client_server.h"


int client_ipc_id, server_msgid;
MessageBuffer *message;

// For unblocking input, because sending STOP via IPC and doing the same via signal would be
struct timeval tv;
fd_set readfds;
int retval;
int fd = 0;



key_t server_key, key;

int my_id;

void stop_client();
void initialize_client_connection();

int main()
{
    initialize_client_connection();

    // nonblocking input setting
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);

    char line[1024];
    char cmd[32];
    char string[1024];
    int id;

    while (1)
    {

        if (msgrcv(client_ipc_id, message, MSG_SIZE, 0, IPC_NOWAIT) != -1)
        {

            if (message->mesg_type == STOP)
            {

                stop_client();
            }

            if (message->mesg_type == TOALL || message->mesg_type == TOONE)
            {
                printf("New message!\n");
                printf("Message from: %d.\n", message->client_id);
                printf("Message body: \"%s\"\n", message->message);
                printf("Server  time: %d-%02d-%02d %02d:%02d:%02d\n", message->tm.tm_year + 1900, message->tm.tm_mon + 1, message->tm.tm_mday, message->tm.tm_hour, message->tm.tm_min, message->tm.tm_sec);
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
            if(fgets(line, sizeof(line), stdin) == NULL){
                perror("fgets() error\n");
            }
            if (strcmp(line, "LIST\n") == 0)
            {
                message->client_id = my_id;
                message->mesg_type = LIST;
                message->dest = -1;
                message->message[0] = '\0';
                msgsnd(server_msgid, message, MSG_SIZE, 0);
            }
            else if (strcmp(line, "STOP\n") == 0)
            {
                message->client_id = my_id;
                message->mesg_type = STOP;
                message->dest = -1;
                message->message[0] = '\0';
                msgsnd(server_msgid, message, MSG_SIZE, 0);
                break;
            }
            else
            {
                if (sscanf(line, "%s %d %s", cmd, &id, string) == 3)
                {
                    if (strcmp(cmd, "2ONE") == 0)
                    {
                        message->client_id = my_id;
                        message->mesg_type = TOONE;
                        strcpy(message->message, string);
                        message->dest = id;
                        msgsnd(server_msgid, message, MSG_SIZE, 0);
                    }
                }
                else if (sscanf(line, "%s %s", cmd, string) == 2)
                {
                    if (strcmp(cmd, "2ALL") == 0)
                    {
                        message->client_id = my_id;
                        message->mesg_type = TOALL;
                        strcpy(message->message, string);
                        msgsnd(server_msgid, message, MSG_SIZE, 0);
                    }
                }
            }

            line[0] = '\0';
        }
    }

    msgctl(client_ipc_id, IPC_RMID, NULL);

    return 0;
}



void stop_on_server_overflow(){
    msgctl(client_ipc_id, IPC_RMID, NULL);
    free(message);
    exit(0);
}

void stop_client()
{
    printf("Received STOP. Exiting...\n");
    fflush(stdout);
    
    message->mesg_type = STOP;
    message->client_id = my_id;
    msgsnd(server_msgid, message, MSG_SIZE, 0);

    free(message);
    msgctl(client_ipc_id, IPC_RMID, NULL);
    exit(0);
}

void initialize_client_connection()
{
    signal(SIGINT, stop_client);

    server_key = ftok(getenv("HOME"), PROJ_ID);
    server_msgid = msgget(server_key, 0666);

    srand(time(NULL));
    key = ftok(getenv("HOME"), rand() % 255 + 1);
    client_ipc_id = msgget(key, 0666 | IPC_CREAT);

    message = malloc(MSG_SIZE);

    message->mesg_type = INIT;
    message->client_key = key;

    msgsnd(server_msgid, message, MSG_SIZE, 0);
    msgrcv(client_ipc_id, message, MSG_SIZE, INIT, 0);
    my_id = message->client_id;

    if (message->client_id == MAX_NO_CLIENTS)
    {
        printf("Server overloaded! Exiting ...\n");
        stop_on_server_overflow();
    }
}
