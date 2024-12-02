#include <stdlib.h>
#include <stdio.h>
#include <serverheaders.h>


user_node_t *new_node(user_t* user) {
    user_node_t *node = malloc(sizeof *node);
    node->user = user;
    node->next = NULL;
    return node;
}

user_t *new_user(char* username, int socket) {
    user_t *user = malloc(sizeof *user);
    user->username = username;
    user->socket = socket;
    return user;
}

void add_user(user_node_t** user_list, char* username, int socket) {
    if (*user_list == NULL) {
        *user_list = new_node(new_user(username, socket));
        return;
    }

    user_node_t* temp = *user_list;
    *user_list = new_node(new_user(username, socket));
    (*user_list)->next = temp;

    return;
}

user_t *remove_user(user_node_t** user_list, int socket) {
    user_node_t *current, *temp;
    user_t *user;
    // Check for null list
    if (*user_list == NULL) {
        return NULL;
    }

    // Check the first node, if it matches return. It might be the last one
    if ((*user_list)->user->socket == socket) {
        temp = *user_list;
        user = temp->user;
        *user_list = (*user_list)->next; // temp->next
        free(temp);
        temp = NULL;
        return user;
    }

    for (current = *user_list; current != NULL; current = current->next) {
        // If it's null, we know we're at the end, and current was checked last iteration 
        if (current->next == NULL) {
            continue;
        }
        // Because we don't have access to previous, we have to work with the next from the current
        if (current->next->user->socket == socket) {
            temp = current->next;
            user = temp->user;
            current->next = current->next->next;
            free(temp);
            temp = NULL;
            return user;
        }

    }

    return NULL;
}