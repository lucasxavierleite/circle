#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "connection.h"

int main(int argc, char *argv[]) {
    int sockfd;
    struct sockaddr_in server_address;

    /* Create the TCP socket */

    tcp_connect(&sockfd, &server_address, PORT);

    /* Bind the socket to specified IP and port */

    bind(sockfd, (struct sockaddr *) &server_address, sizeof(server_address));

    char k;

    while (k != 'q') {
        /* Wait for client to connect */

        printf("Waiting for client...\n");

        listen(sockfd, 1);

        /* Accept connection request from client */

        int client_sockfd = accept(sockfd, NULL, NULL);

        /* Send and print welcome message */

        char welcome_message[] = "Connection established";
        send(client_sockfd, welcome_message, sizeof(welcome_message), 0);
        printf("%s\n", welcome_message);

        int send_status, receive_status;

        /* Communication with client */

        do {
            printf("Waiting for client response...\n");
            system("stty -echo");
            setvbuf(stdout, NULL, _IONBF, 0);
            receive_status = receive_message(client_sockfd);
            system("stty echo");
            setvbuf(stdout, NULL, _IOFBF, 0);
            fflush(stdout);
            send_status = send_message(client_sockfd, "Server");
            fflush(stdout);

        } while (send_status != -1 && receive_status != -1);

        /* Close the client socket */

        close(client_sockfd);

        printf("The client disconnected. Press 'q' to close the server or any other key to start over\n");
        k = getchar();
    }

    /* Close the server socket */

    close(sockfd);

    return 0;
}
