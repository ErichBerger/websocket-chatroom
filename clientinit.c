#include <clientheaders.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
/* Takes in the IP and PORT, connects to the server at that address.*/
int client_init(char* ip, char* port) {
    struct addrinfo hints, *server_info, *p;
    int status, server;

    memset(&hints, 0, sizeof hints);

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    // Get info in form the computer understands
    if ((status = getaddrinfo(ip, port, &hints, &server_info)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }

    for (p = server_info; p != NULL; p = p->ai_next) {
        if ((server = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("socket error");
            continue;
        }
        // Connect the socket
        if (connect(server, p->ai_addr, p->ai_addrlen) == -1) {
            close(server);
            perror("connect error");
            continue;
        }

        break;
    }
    
    if (p == NULL) {
        fprintf(stderr, "failed to connect\n");
        exit(2);
    }

    freeaddrinfo(server_info);

    return server;
}


