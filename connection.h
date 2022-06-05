#ifndef CONNECTION_H
#define CONNECTION_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 8080

int tcp_connect(int * network_socket, struct sockaddr_in * socket_addr, int port);
int receive_message(int sockfd);
int send_message(int sockfd, char * sender);

#endif
