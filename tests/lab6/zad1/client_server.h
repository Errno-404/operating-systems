#define PROJ_ID 1

struct MessageBuffer {
    long mesg_type;
    key_t client_key;
    int client_id;
};



typedef enum MSG_TYPE{
    INIT = 1
} MSG_TYPE;

typedef struct MessageBuffer MessageBuffer;