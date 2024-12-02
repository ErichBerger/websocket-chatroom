#ifndef CLIENT_HEADERS
#define CLIENT_HEADERS
#define BUFFER_SIZE 255
#define COMMAND_TYPE "C"
#define DATA_TYPE "D"
#define COMMAND_CODE 0
#define DATA_CODE 1
#define EX_COMMAND "EX"
#define PM_COMMAND "PM"
#define DM_COMMAND "DM"
#include <pthread.h>
typedef struct node {
    char* message;
    struct node* next;
} node_t;

typedef struct pthread_data {
    node_t** messages;
    int socket;
    int finish;
    pthread_mutex_t *mutex;
    pthread_cond_t *cond;
} pthread_data_t;

/* Takes the ip and port to connect to, returns a
connected socket to the specified server.*/
int client_init(char*, char*);


// MESSAGE FIFO
/* Creates a new node with MESSAGE */
node_t *new_node(char* message);

/* Adds a MESSAGE to the list MESSAGES*/
void enqueue(node_t** messages, char* message);

/* Removes the last message in MESSAGES*/
char *dequeue(node_t **messages);


// HELPERS

/* Places and allocated message from DATA into RETURN_MESSAGE.
 * Returns -1 if there's no data, 0 if connection was closed, and 1 if success. */
int receive_message(pthread_data_t *data, char** return_message);

/* sends MESSAGE encoded with the given TYPE to the socket in DATA. Returns -1 if errors, 1 if success, 0 if no bytes sent. */
int send_message(pthread_data_t *data, char* message, char *type);

/* Strips the first message code from MESSAGE, replaces it with the shortened version.
 * Returns -1 if no types have matched, or the code it parsed. (COMMAND_CODE OR DATA_CODE)*/
int decode_message(char** message);

#endif