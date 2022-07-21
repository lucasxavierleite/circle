#include "server_messages.h"

#include <netinet/in.h>

#include <iostream>
#include <cerrno>
#include <cstring>
#include <string>
#include <sstream>
#include <vector>

namespace circle_server {

    void print_error(const std::string &str) {
        std::cout << "[ERROR " << errno << "] " << str << ": " << std::strerror(errno) << std::endl;
    }

    std::string ctrl_message(enum CTRL_MESSAGE _ctrl_message, std::vector<std::string> arg) {
        std::string message;

        switch (_ctrl_message) {
            case SERVER_EMPTY:
                message = "the room is empty :(. Waiting for new clients";
                break;
            case CLIENT_WELCOME:
                message = "welcome, " + arg[0];
                break;
            case CLIENT_JOIN:
                message = arg[0] + " joined the chat";
                break;
            case CLIENT_LEAVE:
                message = arg[0] + " left the chat";
                break;
            case CLIENT_CHANGE_NICKNAME:
                message = arg[0] + " changed their nickname to " + arg[1];
                break;
            case CLIENT_INVALID_NICKNAME:
                message = "invalid nickname. Please make sure your nickname contains only up to 50 ASCII characters (special characters are not allowed)";
                break;
            case CLIENT_PING:
                message = "pong";
                break;
        }

        return "Server: " + message;
    }

    std::string ctrl_message(enum CTRL_MESSAGE _ctrl_message, const std::string &arg) {
        std::vector<std::string> argv(1);
        argv[0] = arg;

        return ctrl_message(_ctrl_message, argv);
    }

    std::vector<std::string> split_message(const std::string &message) {
        std::stringstream ss(message);

        std::string token;
        std::vector<std::string> split;

        while (std::getline(ss, token, ' ')) {
            split.push_back(token);
        }

        return split;
    }

    std::string format_message(const std::string &nickname, const std::string &message) {
        return nickname + ": " + message;
    }

    void print_message(const std::string &message) {
        std::cout << message << std::endl;
    }

    void clean_str(char (&str)[]) {
        for (size_t i = 0; i < strlen(str); i++)
            if (str[i] == '\n' || str[i] == '\r')
                str[i] = '\0';
    }

    std::string addrtos(struct sockaddr_in address) {
        unsigned oct_1 = address.sin_addr.s_addr & 0xff;
        unsigned oct_2 = (address.sin_addr.s_addr & 0xff00) >> 8;
        unsigned oct_3 = (address.sin_addr.s_addr & 0xff0000) >> 16;
        unsigned oct_4 = (address.sin_addr.s_addr & 0xff000000) >> 24;

        char ipaddr[16];
        sprintf(ipaddr, "%u.%u.%u.%u", oct_1, oct_2, oct_3, oct_4);
        return ipaddr;
    }

}