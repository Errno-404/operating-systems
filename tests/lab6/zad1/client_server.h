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


#define PROJ_ID 1
#define PRIORITY -6

#define MAX_NO_CLIENTS 2




typedef struct MessageBuffer {
    long mesg_type;
    key_t client_key;
    int client_id;
    char message[512];
    struct tm tm;
    int dest;
} MessageBuffer;

const int MSG_SIZE = sizeof(MessageBuffer);


typedef enum MSG_TYPE{
    INIT = 1,
    TOALL = 2,
    TOONE = 3,
    LIST = 4,
    STOP = 5
} MSG_TYPE;



