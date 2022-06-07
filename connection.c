#include "connection.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/*#define MESSAGE_MAX_CHARACTERS 10*/
#define MESSAGE_MAX_CHARACTERS 4096
#define INPUT_MAX_CHARACTERS 10 * MESSAGE_MAX_CHARACTERS

int get_input(char **message) {
    system("stty -icanon");

    *message = malloc(INPUT_MAX_CHARACTERS * sizeof(char));
    fgets(*message, INPUT_MAX_CHARACTERS, stdin);

    return strlen(*message);
}

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

int send_message(int sockfd, char *sender) {
    printf("%s: ", sender);
    /*char message[5008], message_content[4097], c;*/

    /*strcpy(message, sender);*/
    /*char colon[] = ": ";*/
    /*strcat(message, ": ");*/

    char *message;
    int n_of_chars = get_input(&message);

    /* Break message into 4096 characters strings */

    if (n_of_chars > MESSAGE_MAX_CHARACTERS) {
        int n_of_messages = ceil(n_of_chars / (float) MESSAGE_MAX_CHARACTERS);
        char split_messages[n_of_messages][MESSAGE_MAX_CHARACTERS];

        int status = 0;

        for (int i = 0; i < n_of_messages; i++) {
            memcpy(split_messages[i], message + (i * MESSAGE_MAX_CHARACTERS), MESSAGE_MAX_CHARACTERS * sizeof(char));
            status = send(sockfd, &split_messages[i], sizeof(split_messages[i]), MSG_NOSIGNAL);
            if (status == -1)
                return status;
        }

        return status;
    }

    /*scanf("%[^\n]%c", message_content, &c);*/
    /*strcat(message, message_content); */
    return send(sockfd, message, sizeof(message), MSG_NOSIGNAL);
}
