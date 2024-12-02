/**
 * Metro State University
 * ICS 460 Summer 2024
 * Erich Berger
 * StarID: iu8988mn
 * 
 * 
 * 
 * This program acts as a chat server using TCP. With each connection, a thread
 * takes responsibility for dealing with the new client. The server expects the first 
 * data sent to be a username. 
 * 
 * It attempts to log in, if the password correct, it proceeds to give options to the client
 * If the password is incorrect, it logs out
 * If the username doesn't exist, it asks for a new password to create one
 * 
 * It goes into the state of waiting for a request. When it gets one, it handles it, then 
 * waits for another until the user sends EX. They can send a direct message or a public message
 * to other logged in users.
 * 
 * 
 */
#include <serverheaders.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>

pthread_mutex_t client_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t client_cond = PTHREAD_COND_INITIALIZER;
void *thread_handler(void*);
pthread_t threads[THREAD_POOL_SIZE];

int main(int argc, char* argv[]) {
    int server, client;
    struct sockaddr_in client_addr;
    socklen_t client_addr_size = sizeof client_addr;
    client_node_t *client_queue = NULL;
    user_node_t *user_list = NULL;
    pthread_data_t data = {.client_queue = &client_queue, .user_list = &user_list};

    if (argc != 2) {
        printf("Usage: chatserver <port>\n");
        exit(1);
    }

    server = server_init(argv[1]);

    // Start threads
    for (int i = 0; i < THREAD_POOL_SIZE; i++) {
        threads[i] = pthread_create(&threads[i], NULL, thread_handler, &data);
    }
    printf("Starting server\n");
    // Listen for new connections, pass them off to the queue for handling
    while(1) {

        if (client = accept(server, (struct sockaddr *)&client_addr, &client_addr_size), client == -1) {
            perror("accept error");
            continue;
        }
        pthread_mutex_lock(&client_mutex);
        enqueue(&client_queue, client);
        pthread_mutex_unlock(&client_mutex);
        pthread_cond_signal(&client_cond);

    }


    close(server);
}
// Logic for accepting new clients. If any are in queue, it handles them by
// calling client_handler
void* thread_handler(void* pdata) {
    pthread_data_t *data = (pthread_data_t*)pdata;
    while(1) {
        int client;
        pthread_mutex_lock(&client_mutex);
        if (client = dequeue(data->client_queue), client == -1) {
            pthread_cond_wait(&client_cond, &client_mutex);
        }

        pthread_mutex_unlock(&client_mutex);


        if (client == -1) {
            continue;
        }

        client_handler(data, client);
        cleanup(data, client);

    }
}

