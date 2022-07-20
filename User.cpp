#include "User.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <string>
#include <utility>

namespace circle_server {
    User::User(int &_socket_fd, struct sockaddr_in &_address) {
        socket_fd = _socket_fd;
        address = _address;
    }

    bool operator==(const User &a, const User &b) { return a.socket_fd == b.socket_fd; }
    bool operator!=(const User &a, const User &b) { return a.socket_fd != b.socket_fd; }
    bool operator>(const User &a, const User &b) { return a.socket_fd > b.socket_fd; }
    bool operator>=(const User &a, const User &b) { return a.socket_fd >= b.socket_fd; }
    bool operator<(const User &a, const User &b) { return a.socket_fd < b.socket_fd; }
    bool operator<=(const User &a, const User &b) { return a.socket_fd <= b.socket_fd; }

    std::string User::get_nickname() {
        return nickname;
    }

    void User::set_nickname(std::string _nickname) {
        nickname = std::move(_nickname);
    }

    int User::get_socket_fd() {
        return socket_fd;
    }

    struct sockaddr_in User::get_address() {
        return address;
    }
}
