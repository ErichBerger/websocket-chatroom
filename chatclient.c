/**
 * Metro State University
 * ICS 460 Summer 2024
 * Erich Berger
 * StarID: iu8988mn
 * 
 * This program acts as the client to the corresponding chat server. It
 * is responsible for sending the username, taken from the command line
 * as its first message to the server. Upon successfull login or
 * registration, it then goes into a loop of waiting for a new message
 * and dealing with it. A Data message is simply printed, while a Command
 * message means that it should ask the user for input to send to the
 * server.
 * 
 * Commands include DM for Direct Message, PM for Public Message, and EX to
 * exit.
 * 
 * The program uses the main thread to collect incoming messages, and a 
 * child thread deal with the messages.
 */

#include <stdio.h>
#include <stdlib.h>
#include <clientheaders.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>


const char* command_codes[] = {EX_COMMAND, DM_COMMAND, PM_COMMAND};
const int command_codes_size = (sizeof(command_codes)) / (sizeof(*command_codes));
void* handle(void*);

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition_var = PTHREAD_COND_INITIALIZER;

int main(int argc, char* argv[]) {
    int server = 0, result = 0;
    node_t* messages = NULL;
    // Check to see if right amount of args
    if (argc != 4) {
        printf("Usage: chatclient <ip> <host> <username>");
        exit(1);
    }

    // username = strndup(argv[3], 20);
    // Initiate server. Will exit from inside function if needed
    server = client_init(argv[1], argv[2]);
    // printf("server: %d\nusername: %s\n", server, username);

    // Create data for thread, send thread data
    pthread_data_t data = {.messages = &messages, .socket = server, .finish = 0, .mutex = &mutex, .cond = &condition_var};


    // Send username to server. Done here because if it can't send, what's the point of continuing
    
    result = send_message(&data, argv[3], DATA_TYPE);

    if (result <= 0) {
        exit(1);
    }
    pthread_t t;
    pthread_create(&t, NULL, handle, &data);
    while(data.finish != 1) {
        char* message;
        if (result = receive_message(&data, &message), result <= 0) {
            data.finish = 1;
            printf("Connection to server closed. Finish last input if needed.\n");
            break;
        }
        pthread_mutex_lock(&mutex);
        enqueue(&messages, message);
        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&condition_var);
        // Check if message is to exit
        if (strncmp(message, "CEX", 3) == 0) {
            break;
        }
    }
    close(server);
    // Just in case thread was sleeping
    pthread_cond_signal(&condition_var);
    // Join the thread
    pthread_join(t, NULL);

    printf("Program done\n");
    return 0;
}

void *handle(void* pdata) {
    pthread_data_t *data = (pthread_data_t*)pdata;
    if (data == NULL) {
        return NULL;
    }
    int type, result;

    while(data->finish != 1 || *(data->messages) != NULL) {
        char *message;
        // Dequeue message. If null then wait for signal.
        pthread_mutex_lock(&mutex);
        if(message = dequeue(data->messages), message == NULL) {
            pthread_cond_wait(&condition_var, &mutex);
        }
        pthread_mutex_unlock(&mutex);
        // Check if message is null before doing anything else. 
        if(message == NULL) {
            continue;
        }
        // Decode message, return if there's a problem
        if(type = decode_message(&message), type == -1) {
            fprintf(stderr, "Error parsing message type from server\n");
        }

        // Check to see if exit command
        if (strncmp(message, EX_COMMAND, 2) == 0 && type == COMMAND_CODE) {
            printf("Exit received from server.\n");
            data->finish = 1;
            free(message);
            continue;
        }

        // Print the message
        printf("%s", message);

        // Check if data. If so, no need for anything else
        if (type == DATA_CODE) {
            free(message);
            continue;
        }
        
        // We know it's a command message, so check for EX from server

        free(message);
        message = NULL;

        // Before continuing on to handle request from server, check to see if it's closed. 
        if (data->finish == 1) {
            continue;
        }
        /* Might make sense as its own function*/
        // Set new variables needed. Nice not to declare first in case of early returns
        char buffer[BUFFER_SIZE] = {0}, *type = NULL;

        // Get response
        fgets(buffer, 100, stdin);
        message = strndup(buffer, BUFFER_SIZE -1);
        if (message == NULL) {
            message = " "; 
        }

        // Double check command codes by cycling through
        for (int i = 0; i < command_codes_size; i++) {
            if (strncmp(message, command_codes[i], 2) == 0) {
                type = COMMAND_TYPE;
            }
        }
        // If message didn't match any commands, it should be treated as data
        if (type == NULL) {
            type = DATA_TYPE;
        }
        // If the message is to exit, set finish
        if (strncmp(message, EX_COMMAND, 2) == 0) {
            // Because of how I want the server to confirm the closure, this is moot
        }

        // send send_message
        if (result = send_message(data, message, type), result <= 0) {
            printf("Connection to socket closed.");
            data->finish = 1;
        }

        free(message);

    }
    printf("Now returning to main thread\n");
    return NULL;
}