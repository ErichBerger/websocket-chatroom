#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <serverheaders.h>
#include <sys/socket.h>


int receive_message(int sockfd, char **return_message) {
    ssize_t bytes_read;
    char buffer[BUFFER_LENGTH] = {0};

    if (return_message == NULL) {
        fprintf(stderr, "receive_message: must pass in an address to a pointer\n");
        return 0;
    }

    if (bytes_read = recv(sockfd, buffer, BUFFER_LENGTH, 0), bytes_read <= 0) {
        return bytes_read;
    }

    buffer[BUFFER_LENGTH] = '\0';

    *return_message = strndup(buffer, BUFFER_LENGTH -1 );
    if (*return_message == NULL) {
        fprintf(stderr, "strndup: could not allocate from buffer\n");
        return -1;
    }

    return 1;
}

int send_message(int sockfd, char* message, char* type) {
    char buffer[BUFFER_LENGTH] = {0};
    int result;
    if (message == NULL || type == NULL) {
        fprintf(stderr, "send_message requires type to be 'C' or 'D'\n");
        return -1;
    }

    strncat(buffer, type, strlen(type));
    strncat(buffer, message, BUFFER_LENGTH - 2);

    if (result = send(sockfd, buffer, strlen(buffer) + 1, MSG_DONTWAIT), result <= 0) {
        return result;
    }

    //printf("Message sent to %d:%s\n", sockfd, buffer);
    sleep(1UL);
    return 1;
    
}


int decode_message(char** message) {
    char *type = strndup(*message, 1);

    // Allocate enough memory to shorten the message by 1
    char* temp = strndup(*message + 1, strlen(*message + 1));
    // Copy the message, ignoring the code at message[0]
    free(*message);

    *message = temp;
    
    if (strncmp(type, COMMAND_TYPE, 1) == 0) {
        free(type);
        return COMMAND_CODE;
    }

    if (strncmp(type, DATA_TYPE, 1) == 0) {
        free(type);
        return DATA_CODE;
    }
    free(type);
    return -1;
}


void cleanup(pthread_data_t * data, int socket) {
    // Make sure we're not freeing any invalid pointers, now
    user_t * user = remove_user(data->user_list, socket);

    if (user == NULL) {
        return;
    }

    if (user->username != NULL) {
        free(user->username);
    }

    free(user);
    close(socket);
    return;
}