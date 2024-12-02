#include <clientheaders.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>

node_t *new_node(char* message) {
    node_t *node = malloc(sizeof *node);
    node->message = message;
    node->next = NULL;
    return node;
}
void enqueue(node_t** messages, char* message) {
    if(*messages == NULL) {
        *messages = new_node(message);
        return;
    }

    node_t *temp = *messages;
    *messages = new_node(message);
    (*messages)->next = temp;

    return;
}

char *dequeue(node_t **messages) {
    if (*messages == NULL) {
        return NULL;
    }

    if ((*messages)->next == NULL) {
        char *temp = (*messages)->message;
        node_t *temp_node = *messages;
        *messages = NULL;
        free(temp_node);
        return temp;
    }

    for(node_t* m = *messages; m != NULL; m = m->next) {
        if (m->next->next == NULL) {
            char *temp = m->next->message;
            node_t *temp_node = m->next;
            m->next = NULL;
            free(temp_node);
            return temp;
        }
    }

    return NULL;

}
