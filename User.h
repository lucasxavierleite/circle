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
        std::string channel;
        bool admin;
        bool mute;
        bool kicked;

    public:
        User(int &_socket_fd, struct sockaddr_in &_address);
        struct sockaddr_in get_address();
        std::string get_nickname();
        void set_nickname(std::string _nickname);
        std::string get_channel();
        void set_channel(const std::string &channel);
        bool is_admin() const;
        void set_admin(const bool &_admin);
        bool is_mute() const;
        void set_mute(const bool &_mute);
        bool is_kicked() const;
        void set_kicked(const bool &_kicked);
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
