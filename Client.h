#ifndef CIRCLE_CLIENT_H
#define CIRCLE_CLIENT_H

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <string>

#define SERVER_IP_ADDRESS "127.0.0.1"
#define SERVER_PORT 9999
#define MESSAGE_MAX_CHARACTERS 4096

namespace circle_client {

    void *handle_recv(void *arg);

    class Client {
    private:
        bool connected;
        int socket_fd;
        std::string nickname;
        pthread_t recv_thread;
        pthread_mutex_t mutex{};

    public:
        Client();
        int get_socket_fd() const;
        std::string get_nickname();
        void set_nickname(std::string &_nickname);
        void connect();
        void disconnect();
        bool is_connected() const;
        int init();
    };

}

#endif