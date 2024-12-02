#include <serverheaders.h>
#include <stdlib.h>
#include <unistd.h>

client_node_t* new_client_node(int socket) {
    client_node_t* n = malloc (sizeof *n);
    n->socket = socket;
    n->next = NULL;
    return n;
}

void enqueue(client_node_t **client_queue, int socket) {
    if (*client_queue == NULL) {
        *client_queue = new_client_node(socket);
        return;
    }

    client_node_t* temp = *client_queue;
    *client_queue = new_client_node(socket);
    (*client_queue)->next = temp;

}

int dequeue(client_node_t **client_queue) {
    int socket;
    // If queue is empty
    if (*client_queue == NULL) {
        return -1;
    }
    // If only one node exists
    if ((*client_queue)->next == NULL) {
        socket = (*client_queue)->socket;
        free(*client_queue);
        *client_queue = NULL;
        return socket;
    }

    for(client_node_t* c = *client_queue; c != NULL; c = c->next) {
        if (c->next->next == NULL) {
            socket = c->next->socket;
            client_node_t *temp = c->next;
            c->next = NULL;
            free(temp);
            return socket;
            
        }
    }

    return -1;
}