#include "Client.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <string>
#include <utility>

namespace circle {
    Client::Client(int &_socket_fd, struct sockaddr_in &_address) {
        socket_fd = _socket_fd;
        address = _address;
    }

    bool operator==(const Client &a, const Client &b) { return a.socket_fd == b.socket_fd; }
    bool operator!=(const Client &a, const Client &b) { return a.socket_fd != b.socket_fd; }
    bool operator>(const Client &a, const Client &b) { return a.socket_fd > b.socket_fd; }
    bool operator>=(const Client &a, const Client &b) { return a.socket_fd >= b.socket_fd; }
    bool operator<(const Client &a, const Client &b) { return a.socket_fd < b.socket_fd; }
    bool operator<=(const Client &a, const Client &b) { return a.socket_fd <= b.socket_fd; }

    std::string Client::get_nickname() {
        return nickname;
    }

    void Client::set_nickname(std::string _nickname) {
        nickname = std::move(_nickname);
    }

    int Client::get_socket_fd() {
        return socket_fd;
    }

    struct sockaddr_in Client::get_address() {
        return address;
    }
}
