#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "client_server.h"
  
// structure for message queue

  


int main()
{   
    srand(time(NULL));

    // po uruchomieniu klienta pobieramy kolejkę serwera
    key_t server_key = ftok(getenv("HOME"), PROJ_ID);
    int server_msgid = msgget(server_key, 0666);


    // tworzymy własną kolejkę
    key_t key = ftok(getenv("HOME"), rand() % 255 + 1);
    int msgid = msgget(key, 0666 | IPC_CREAT);


    // wysyłamy message do servera ze swoim key (póki co INIT = 1)
    MessageBuffer *message = malloc(sizeof(MessageBuffer));
    
    message->mesg_type = INIT;
    message->client_key = key;

    msgsnd(server_msgid, message, sizeof(MessageBuffer), 0);
    msgrcv(msgid, message, sizeof(MessageBuffer), INIT, 0);
    printf("%d", message->client_id);



    char input[16];
    while(1){
        scanf("%s", input);
        if(strcmp(input, "LIST") == 0){
            printf("OK");
            fflush(stdout);
            message->mesg_type = LIST;
            msgsnd(server_msgid, message, sizeof(MessageBuffer), 0);
        }
        
        
    }
    



    msgctl(msgid, IPC_RMID, NULL);


    return 0;
}