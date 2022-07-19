#ifndef CIRCLE_CLIENT_H
#define CIRCLE_CLIENT_H

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <string>

namespace circle {

    class Client {
    private:
        int socket_fd;
        struct sockaddr_in address{};
        std::string nickname;

    public:
        Client(int &_socket_fd, struct sockaddr_in &_address);
        struct sockaddr_in get_address();
        std::string get_nickname();
        void set_nickname(std::string _nickname);
        int get_socket_fd();
        friend bool operator==(const Client &a, const Client &b);
        friend bool operator!=(const Client &a, const Client &b);
        friend bool operator>(const Client &a, const Client &b);
        friend bool operator>=(const Client &a, const Client &b);
        friend bool operator<(const Client &a, const Client &b);
        friend bool operator<=(const Client &a, const Client &b);
    };
}


#endif
