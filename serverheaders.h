#ifndef SERVER_HEADERS
#define SERVER_HEADERS

#define BUFFER_LENGTH 255
#define THREAD_POOL_SIZE 10
#define DATA_TYPE "D"
#define COMMAND_TYPE "C"
#define COMMAND_CODE 0
#define DATA_CODE 1
#define EX_COMMAND "EX"
#define DM_COMMAND "DM"
#define PM_COMMAND "PM"
#define USER_INFO_LIMIT 20
#define USER_FILE "users.txt"

typedef struct client_node {
    int socket;
    struct client_node * next;
} client_node_t;

typedef struct user {
    int socket;
    char* username;
} user_t;

typedef struct user_node {
    user_t *user;
    struct user_node *next;
} user_node_t;

typedef struct pthread_data {
    client_node_t ** client_queue;
    user_node_t ** user_list;

} pthread_data_t;

/* SERVER INIT */

/* Takes in port, returns socket id of a listening socket. */
int server_init(char* port);

/* HELPERS*/

/* Takes in a socket to receive from, assigns the return_message in the form of malloc.
 * returns -1 if timeout. */
int receive_message(int sockfd, char **return_message);

/* Takes in a socket to send to, a message, and the data type to prepend.
 * Returns -1 if errors, 1 for success. */
int send_message(int sockfd, char* send_message, char* type);

/* Decodes MESSAGE. Returns -1 for errors, otherwise message type. */
int decode_message(char** message);

/* Frees any logged in user, closes socket */
void cleanup(pthread_data_t* data, int socket);

/* CLIENT QUEUE*/

void enqueue(client_node_t **client_queue, int socket);

/* Returns oldest client's socket number, -1 if empty*/
int dequeue(client_node_t **client_queue);

/* USER LIST */

// Takes in a username and socket to add as a user to the list of logged on users
void add_user(user_node_t **user_list, char* username, int socket);

// Looks in USER_LIST to find a user that matches SOCKET, returns that user
user_t* remove_user(user_node_t **user_list, int socket);


/* HANDLERS*/

// Handles client stuff. On return, socket has not been closed
void client_handler(pthread_data_t *data, int socket);

/* Handles a PM. If another command is entered during its processing, it returns the command message
 * Returns null on errors, "" empty string on success, indicating another command is needed. */
char* dm_handler(pthread_data_t *data, int socket, char* username);

/* Handles a DM. If another command is entered during its processing, it returns the command message
 * Returns null on errors, "" empty string on success, indicating another command is needed. */
char* pm_handler(pthread_data_t *data, int socket, char* username);


#endif