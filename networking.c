#include "networking.h"

int server_setup() {
    //use getaddrinfo
    struct addrinfo * hints, * results;
    hints = calloc(1,sizeof(struct addrinfo));
    hints->ai_family = AF_INET;
    hints->ai_socktype = SOCK_STREAM; //TCP socket
    hints->ai_flags = AI_PASSIVE; //only needed on server
    if (getaddrinfo(NULL, "9845", hints, &results) == -1) {
        printf("Error on getaddrinfo\n");
        return -1;
    }

    //create socket
    int sd = socket(results->ai_family, results->ai_socktype, results->ai_protocol);
    if (sd==-1) {
        printf("Error on socket\n");
        return -1;
    }

    if (bind(sd, results->ai_addr, results->ai_addrlen) == -1) {
        printf("Error on bind\n");
        return -1;
    }
    if (listen(sd, 100) == -1) {
        printf("Error on listen\n");
        return -1;
    }

    free(hints);
    freeaddrinfo(results);

    return sd;
}

int server_connect(int from_client) {
    int client_socket;
    socklen_t sock_size;
    struct sockaddr_storage client_address;
    sock_size = sizeof(client_address);
    client_socket = accept(from_client, (struct sockaddr *)&client_address, &sock_size);
    if (client_socket == -1) {
        printf("Error on accept\n");
        return -1;
    }

	return client_socket;
}

int client_handshake() {
    //use getaddrinfo
    struct addrinfo * hints, * results;
    hints = calloc(1,sizeof(struct addrinfo));
    hints->ai_family = AF_INET;
    hints->ai_socktype = SOCK_STREAM; //TCP socket
    hints->ai_flags = AI_PASSIVE; //only needed on server
    if (getaddrinfo("127.0.0.1", "9845", hints, &results) == -1) {
        printf("Error on getaddrinfo\n");
        return -1;
    }

    //create socket
    int sd = socket(results->ai_family, results->ai_socktype, results->ai_protocol);
    if (sd==-1) {
        printf("Error on socket\n");
        return -1;
    }

    if (connect(sd, results->ai_addr, results->ai_addrlen) == -1) {
        printf("Error on connect\n");
        return -1;
    }

    free(hints);
    freeaddrinfo(results);

    return sd;
}

void writeint(int fd, int x) { write(fd, &x, sizeof(int)); }
