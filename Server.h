#ifndef CIRCLE_SERVER_H
#define CIRCLE_SERVER_H

#include "User.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <string>
#include <vector>

#define MAX_CLIENTS_CONNECTED 10
#define MAX_SEND_ATTEMPTS 5

namespace circle_server {

    class Server {
    private:
        static pthread_t tid;
        static int socket_fd;
        static struct sockaddr_in address;
        static const char *ip_address;
        static int port;
        static pthread_mutex_t clients_mutex;
        static std::vector<User> clients;
        static void *handle_client(void *arg);
        static bool log;

    public:
        explicit Server();
        static void set_port(char *&_port);
        static void listen();
        static void add_client(User &client);
        static void remove_client(User &client);
        static void send_message(User &client, const std::string &message);
        static void send_response_message(User &client, const std::string &message);
        static void print_log(const std::string &log_message);
    };
}

#endif