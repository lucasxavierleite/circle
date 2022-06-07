#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "connection.h"

int main(int argc, char *argv[]) {
    int sockfd;
    struct sockaddr_in server_address;

    /* Check for error with the connection */

    if (tcp_connect(&sockfd, &server_address, PORT) == -1) {
        perror("Couldn't connect to the remote socket");
        return -1;
    }

    /* Receive data from server */
    char server_response[256];
    recv(sockfd, &server_response, sizeof(server_response), 0);

    /* Print out the server's response */

    printf("%s\n", server_response);

    int send_status, receive_status;

    /* Communication with server */

    do {
        fflush(stdout);
        send_status = send_message(sockfd, "Client");
        fflush(stdout);
        printf("Waiting for server response...\n");
        system("stty -echo");
        setvbuf(stdout, NULL, _IONBF, 0);
        receive_status = receive_message(sockfd);
        system("stty echo");
        setvbuf(stdout, NULL, _IOFBF, 0);
    } while (send_status != -1 && receive_status != -1);

    perror("Connection closed");

    /* Close the socket */

    close(sockfd);

    return 0;
}
