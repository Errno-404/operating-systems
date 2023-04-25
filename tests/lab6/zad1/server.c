#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include "client_server.h"
#define MAX 10


  



int main()
{
    // tworzymy kolejkę
    key_t key = ftok(getenv("HOME"), PROJ_ID);
    int msgid = msgget(key, 0666 | IPC_CREAT);
  
    // tworzymy strukturę na komunikaty  
    MessageBuffer *message = malloc(sizeof(MessageBuffer));
    

    while(1){
        msgrcv(msgid, message, sizeof(MessageBuffer), 1, 0);
        
        switch (message->mesg_type)
        {
        case 1:
            printf("%d", message->client_id);
            fflush(stdout);
            break;
        
        default:
            break;
        }
        
        
        fflush(stdout);
    }
    // odbieramy komunikat z klienta
    

    

    msgctl(msgid, IPC_RMID, NULL);

  
    return 0;
}