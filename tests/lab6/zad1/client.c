#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include "client_server.h"
  
// structure for message queue

  


int main()
{   
    // po uruchomieniu klienta pobieramy kolejkę serwera
    key_t server_key = ftok(getenv("HOME"), PROJ_ID);
    int server_msgid = msgget(server_key, 0666);


    // tworzymy własną kolejkę
    key_t key = ftok(getenv("HOME"), PROJ_ID % 255 + 1);
    int msgid = msgget(key, 0666 | IPC_CREAT);


    // wysyłamy message do servera ze swoim key (póki co INIT = 1)
    MessageBuffer *message = malloc(sizeof(MessageBuffer));
    
    message->mesg_type = 1;
    message->client_key = key;
    message->client_id = 143;

    msgsnd(server_msgid, message, sizeof(MessageBuffer), 0);

    msgctl(msgid, IPC_RMID, NULL);


    return 0;
}