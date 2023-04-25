#define PROJ_ID 1
#define PRIORITY -6
typedef struct MessageBuffer {
    long mesg_type;
    key_t client_key;
    int client_id;
} MessageBuffer;



typedef enum MSG_TYPE{
    INIT = 1,
    LIST = 2
} MSG_TYPE;
