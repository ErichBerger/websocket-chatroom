#include <serverheaders.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int verify_user(pthread_data_t*, int, char*);
int register_user(pthread_data_t*, int, char*);


void client_handler(pthread_data_t *data, int socket) {
    int result;
    char* username;
    // When the client starts, it automatically sends the username
    if (result = receive_message(socket, &username), result <= 0) {
        return;
    }
    // If not data return
    if (result = decode_message(&username), result <= 0) {
        free(username);
        return;
    }
    
    // Log in
    result = verify_user(data, socket, username);
    // If the user existed, but entered bad info, send message and close.
    if (result == 0) {
        free(username);
        if (result = send_message(socket, "\nProblem entering credentials, please try again\n", DATA_TYPE), result <= 0) {
            return;
        }
        if (result = send_message(socket, EX_COMMAND, COMMAND_TYPE), result <= 0) {
            return;
        }
        return;
    }
    // If no user was found, attempt to register them. Exit if not able
    if (result == -1) {
        if (result = register_user(data, socket, username), result == 0) {
            free(username);
            if (result = send_message(socket, "\nProblem registering user, please try again\n", DATA_TYPE), result <= 0) {
                return;
            }
            if (result = send_message(socket, EX_COMMAND, COMMAND_TYPE), result <= 0) {
                return;
            }
            return;
        }
    }

    // NOTE: at this point the user has been added to the list, so no need to free username when returning

    // Send message confirming login
    if (result = send_message(socket, "\nSuccessfully logged in!\n", DATA_TYPE), result <= 0) {
        return;
    }

    
    char* command = "";

    while(1) {
        // Get new command if message is blank
        if (strncmp(command, "", 1) == 0) {

            if (result = send_message(socket, "\nEnter a command.\n\nDM for Direct Message\nPM for Public Message\nEX to Exit\nPress <ENTER> to refresh messages\n\n", COMMAND_TYPE), result <= 0 ) {
                return;
            }

            if (result = receive_message(socket, &command), result <= 0) {
                return;
            }

            if (result = decode_message(&command), result == -1) {
                return;
            }
        }
        // If command matches DM, handle it and continue with new command
        if (strncmp(command, DM_COMMAND, 2) == 0) {
            free(command);
            if (command = dm_handler(data, socket, username), command == NULL) {
                return;
            }
            continue;
        } 
        // If command matches PM, handle it and continue with new command
        if (strncmp(command, PM_COMMAND, 2) == 0) {
            free(command);
            if (command = pm_handler(data, socket, username), command == NULL) {
                return;
            }
            continue;
        } 
        // If command matches EX, return
        if (strncmp(command, EX_COMMAND, 2) == 0){
            free(command);
            send_message(socket, EX_COMMAND, COMMAND_TYPE);
            return;
        } 

        // If command didn't match anything, inform the user
        free(command);
        if (result = send_message(socket, "\nPlease enter a valid command\n", DATA_TYPE), result <= 0) {
            return;
        }

        command = "";
        
    }
    return;
}

// Returns -1 if they don't exist, 0 for failure, 1 for success
int verify_user(pthread_data_t *data, int socket, char* username) {
    int result;
    FILE *file = fopen(USER_FILE, "r");
    char *password;
    if(file == NULL) {
        fprintf(stderr, "error opening file\n");
        return 0;
    }

    char file_username[USER_INFO_LIMIT] = {0};
    char file_password[USER_INFO_LIMIT] = {0};

    while (result = fscanf(file, "%s %s", file_username, file_password), result == 2) {
        if (strncmp(username, file_username, USER_INFO_LIMIT -1) == 0) {
            break;
        }
        memset(file_username, '\0', USER_INFO_LIMIT);
    }
    // If we reached the end of the file without breaking, nothing was found to match.
    if (result == EOF) {
        return -1;
    }
    fclose(file);
    // Get password
    if (result = send_message(socket, "\nEnter your password\n", COMMAND_TYPE), result <= 0) {
        return 0;
    }
    
    if (result = receive_message(socket, &password), result <= 0) {
        return 0;
    }

    // If it's a command, it's invalid because they haven't logged on
    // COMMAND_CODE expands to 0, so the <= 0 works here
    if (result = decode_message(&password), result <= 0) {
        return 0;
    }

    // Get rid of newline at end

    if (password[strlen(password) - 1] == '\n') {
        password[strlen(password) -1] = '\0';
    }
    

    if (result = strncmp(password, file_password, USER_INFO_LIMIT -1), result == 0) {
        add_user(data->user_list, username, socket);
    }

    free(password);
    return result == 0;
}

/* Gets password, registers user and adds them to logged in list.
 * Returns 0 for failure, 1 for success */
int register_user(pthread_data_t *data, int socket, char* username) {
    int result;
    char *password = NULL;
    
    if (result = send_message(socket, "\nNo user found, enter new password to create new account\n", COMMAND_TYPE), result <= 0) {
        return 0;
    }

    if (result = receive_message(socket, &password), result <= 0) {
        return 0;
    }

    if (result = decode_message(&password), result <= 0 ) {
        return 0;
    }


    FILE *file = fopen(USER_FILE, "a");

    if (file == NULL) {
        free(password);
        return 0;
    } 

    fprintf(file, "%s %s", username, password);

    fclose(file);

    free(password);

    add_user(data->user_list, username, socket);

    return 1;

}

char *dm_handler(pthread_data_t *data, int socket, char* username) {
    char buffer[BUFFER_LENGTH] = {0}, *message = NULL, *receiving_username = NULL;
    
    int result;

    // Build list of users online

    strncat(buffer, "\nUsers currently online:\n", BUFFER_LENGTH - strlen(buffer) -1);
    for (user_node_t* p = *(data->user_list); p != NULL; p = p->next) {
        strncat(buffer, p->user->username, (BUFFER_LENGTH - strlen(buffer) -1));
        strncat(buffer, "\n", (BUFFER_LENGTH - strlen(buffer) -1));
    }

    strncat(buffer, "\nEnter the user you wish to message:\n", BUFFER_LENGTH - strlen(buffer) - 1);
    // Send list of users online
    if (result = send_message(socket, buffer, COMMAND_TYPE), result <= 0) {
        return NULL;
    }
    // Get user response
    if (result = receive_message(socket, &receiving_username), result <= -1) {
        return NULL;
    }

    if (result = decode_message(&receiving_username), result == -1) {
        free(receiving_username);
        return NULL;
    }
    // If user entered another command, return the command
    if (result == COMMAND_CODE) {
        return receiving_username;
    }
    // Strip newline for comparison
    if (receiving_username[strlen(receiving_username) -1] == '\n') {
        receiving_username[strlen(receiving_username) -1] = '\0';
    }

    // Get message to send
    if (result = send_message(socket, "\nEnter your message:\n", COMMAND_TYPE), result <= 0) {
        free(receiving_username);
        return NULL;
    }
    if (result = receive_message(socket, &message), result <= -1) {
        free(receiving_username);
        return NULL;
    }

    if (result = decode_message(&message), result == -1) {
        free(receiving_username);
        free(message);
        return NULL;
    }
    // If user entered another command, return the command
    if (result == COMMAND_CODE) {
        free(receiving_username);
        return message;
    }

    // Stylize message
    memset(buffer, '\0', BUFFER_LENGTH);
    snprintf(buffer, BUFFER_LENGTH -1, "\nDM from %s: %s", username, message);
    free(message);

    // Cycle through user list, sending the message to the appropriate client if a match
    user_node_t *p;
    for (p = *(data->user_list); p != NULL; p = p->next) {
        if (strncmp(receiving_username, p->user->username, USER_INFO_LIMIT -1) == 0) {
            // If match, send message
            if (result = send_message(p->user->socket, buffer, DATA_TYPE), result <=0) {
                send_message(socket, "\nDM failed to send, user may have logged out.\n", DATA_TYPE);
            }
            send_message(socket, "\nDM sent\n", DATA_TYPE);
            break;
        }
    }
    // If no users found, send message.
    if (p == NULL) {
        send_message(socket, "\nNo user found with that username\n", DATA_TYPE);
    }

    free(receiving_username);
    return "";
}

char *pm_handler(pthread_data_t *data, int socket, char* username) {
    char *message, buffer[BUFFER_LENGTH] = {0};
    int result;
    if (result = send_message(socket, "\nEnter your PM:\n", COMMAND_TYPE), result <= 0) {
        return NULL;
    }
    // Get user response
    if (result = receive_message(socket, &message), result <= -1) {
        return NULL;
    }

    if (result = decode_message(&message), result == -1) {
        free(message);
        return NULL;
    }
    // If user entered another command, return the command
    if (result == COMMAND_CODE) {
        return message;
    }


    snprintf(buffer, BUFFER_LENGTH -1, "\nPM from %s: %s", username, message);
    free(message);
    user_node_t *p = NULL;
    for (p = *(data->user_list); p != NULL; p = p->next) {
        result = send_message(p->user->socket, buffer, DATA_TYPE);
        if (result <= 0) {
            printf("Message from %s failed to send to %s\n", username ,p->user->username);
        }
    }

    if (result = send_message(socket, "\nPM sent\n", DATA_TYPE), result <= 0) {
        return NULL;
    }
    return "";
}