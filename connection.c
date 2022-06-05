#include "connection.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int tcp_connect(int * sockfd, struct sockaddr_in * socket_addr, int port) {
    /* Create a TCP socket */
    *sockfd = socket(AF_INET, SOCK_STREAM, 0);

    /* Specify an address for the socket */
    socket_addr->sin_family = AF_INET;
    socket_addr->sin_port = htons(port);
    socket_addr->sin_addr.s_addr = INADDR_ANY;

    return connect(*sockfd, (struct sockaddr *) socket_addr, sizeof(*socket_addr));
}

/* 5008 = 9 + 2 + 4096 + 1
 * [nickname][:][ ][message][\0]*/

int receive_message(int sockfd) {
    char message[5008];
    int status = recv(sockfd, &message, sizeof(message), 0);
    printf("%s\n", message);
    return status;
}

int send_message(int sockfd, char sender[10]) {
    printf("%s: ", sender);
    char message[5008], message_content[4097], c;
    strcpy(message, sender);
    char colon[] = ": ";
    strcat(message, ": ");
    scanf("%[^\n]%c", message_content, &c);
    strcat(message, message_content); 
    return send(sockfd, &message, sizeof(message), MSG_NOSIGNAL);
}
