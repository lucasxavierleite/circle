#ifndef CIRCLE_SERVER_H
#define CIRCLE_SERVER_H

#include "Client.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <string>
#include <vector>

#define DEFAULT_IP_ADDRESS "127.0.0.1"
#define DEFAULT_PORT 9999

#define NICKNAME_MAX_CHARACTERS 50
#define MESSAGE_MAX_CHARACTERS 4096
#define MAX_CLIENTS_CONNECTED 10

namespace circle {

    class Server {
    private:
        static pthread_t tid;
        static int socket_fd;
        static struct sockaddr_in address;
        static const char *ip_address;
        static int port;
        static pthread_mutex_t clients_mutex;
        static std::vector<Client> clients;
        static void *handle_client(void *arg);
        static bool log;

    public:
        explicit Server();
        static void set_port(char *&_port);
        static void listen();
        static void add_client(Client &client);
        static void remove_client(Client &client);
        static void send_message(Client &client, std::string &message);
        static void send_response_message(Client &client, const std::string &message);
        static void print_log(const std::string &log_message);
    };
}

#endif