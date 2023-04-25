#define PROJ_ID 1

struct MessageBuffer {
    long mesg_type;
    key_t client_key;
    int client_id;
};


typedef struct MessageBuffer MessageBuffer;