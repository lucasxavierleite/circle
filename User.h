#ifndef CIRCLE_USER_H
#define CIRCLE_USER_H

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <string>

namespace circle_server {

    class User {
    private:
        int socket_fd;
        struct sockaddr_in address{};
        std::string nickname;

    public:
        User(int &_socket_fd, struct sockaddr_in &_address);
        struct sockaddr_in get_address();
        std::string get_nickname();
        void set_nickname(std::string _nickname);
        int get_socket_fd();
        friend bool operator==(const User &a, const User &b);
        friend bool operator!=(const User &a, const User &b);
        friend bool operator>(const User &a, const User &b);
        friend bool operator>=(const User &a, const User &b);
        friend bool operator<(const User &a, const User &b);
        friend bool operator<=(const User &a, const User &b);
    };
}


#endif
