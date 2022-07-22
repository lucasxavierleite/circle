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
        channel = {};
        admin = false;
        mute = false;
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

    std::string User::get_channel() {
        return channel;
    }

    void User::set_channel(const std::string &_channel) {
        channel = _channel;
    }

    int User::get_socket_fd() {
        return socket_fd;
    }

    struct sockaddr_in User::get_address() {
        return address;
    }

    void User::set_admin(const bool &_admin) {
        admin = _admin;
    }

    bool User::is_admin() const {
        return admin;
    }

    void User::set_mute(const bool &_mute) {
        mute = _mute;
    }

    bool User::is_mute() const {
        return mute;
    }
}
