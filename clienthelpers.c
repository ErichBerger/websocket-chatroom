#include <clientheaders.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>


int receive_message(pthread_data_t *data, char **return_message) {
    char buffer[BUFFER_SIZE] = {0};
    ssize_t bytes_read;

    if (return_message == NULL) {
        fprintf(stderr, "receive_message: must pass in an address to a pointer\n");
        return 0;
    }
    // Get the message into the buffer
    if (bytes_read = recv(data->socket, &buffer, BUFFER_SIZE-1, 0), bytes_read <= 0) {
        return bytes_read;
    }
    // Create a malloc'd string from the buffer
    buffer[bytes_read] = '\0';
    *return_message = strndup(buffer, BUFFER_SIZE - 1); 

    if (*return_message == NULL) {
        fprintf(stderr, "strndup: could not allocate from buffer\n");
    }
    return 1;

}

int send_message(pthread_data_t *data, char* message, char *type) {
    char buff[BUFFER_SIZE] = {0};

    int result;
    if (strncmp(type, COMMAND_TYPE, 1) != 0 && strncmp(type, DATA_TYPE, 1) != 0) {
        fprintf(stderr, "send_message() requires type to be 'C' or 'D'\n");
        return -1;
    }
    // Encode with the type
    strncat(buff, type, strlen(type));
    strncat(buff, message, BUFFER_SIZE -2);

    // Send the encoded message
    if (result = send(data->socket, buff, strlen(buff) +1, 0), result <= 0) {
        return result;
    }
    // Dealing with stream (hacky, make better buffer)
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


