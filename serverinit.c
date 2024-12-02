#include <serverheaders.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define BACKLOG 10

int server_init(char* port) {
    struct addrinfo hints, *server_info, *p;
    int status, server, yes = 1;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if (status = getaddrinfo(NULL, port, &hints, &server_info), status != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    } 

    for (p = server_info; p != NULL; p = p->ai_next) {
        if (server = socket(p->ai_family, p->ai_socktype, p->ai_protocol), server == -1) {
            perror("socket error");
            continue;
        }

        if (setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == -1) {
            perror("setsockopt error");
            exit(1);
        }

        if (bind(server, p->ai_addr, p->ai_addrlen) == -1) {
            close(server);
            perror("bind error");
            continue;
        }

        break;
    }

    freeaddrinfo(server_info);

    if (p == NULL) {
        fprintf(stderr, "failed to bind\n");
        exit(1);
    }

    if (listen(server, BACKLOG) == -1) {
        perror("listen error");
        close(server);
        exit(1);
    }

    return server;
}