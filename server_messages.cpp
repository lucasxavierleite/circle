#include "server_messages.h"

#include <netinet/in.h>

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cerrno>
#include <cstring>

namespace circle_server {

    void print_error(const std::string &str) {
        std::cout << "[ERROR " << errno << "] " << str << ": " << std::strerror(errno) << std::endl;
    }

    std::string ctrl_message(enum CTRL_MESSAGE _ctrl_message, std::vector<std::string> arg) {
        std::string message;

        switch (_ctrl_message) {
            case WELCOME:
                message = "Welcome, " + arg[0];
                break;
            case JOIN:
                message = arg[0] + " joined the channel " + arg[1];
                break;
            case LEAVE:
                message = arg[0] + " left the channel " + arg[1];
                break;
            case CHANGE_NICKNAME:
                message = arg[0] + " changed their nickname to " + arg[1];
                break;
            case INVALID_NICKNAME:
                message = "Invalid nickname. Please, make sure your nickname contains only up to 50 ASCII characters (special characters are not allowed)";
                break;
            case PING:
                message = "Pong";
                break;
            case INVALID_CHANNEL_NAME:
                message = "Invalid channel. Please, make sure the channel name starts with '#' or '&' and doesn't contain commas (',') or blank spaces";
                break;
            case NO_CHANNEL:
                message = R"(You need to use "/join" to join a channel in order to send messages and perform other actions. See available commands with "/help")";
                break;
            case JOIN_CURRENT_CHANNEL:
                message = "You already are in the channel " + arg[0];
                break;
            case NEW_ADMIN:
                message = "Congratulations! Since you are the first to arrive here, you are now the channel administrator";
                break;
            case NOT_ADMIN:
                message = "You need administrator privileges to perform this action";
                break;
            case MUTE:
                message = "You were muted by the channel administrator";
                break;
            case KICK:
                message = arg[0] + " was kicked out by the channel administrator " + arg[1];
                break;
            case USER_NOT_FOUND:
                message = "User " + arg[0] + " not found";
                break;
        }

        return ">>> " + message;
    }

    std::string ctrl_message(enum CTRL_MESSAGE _ctrl_message, const std::string &arg) {
        std::vector<std::string> argv(1);
        argv[0] = arg;

        return ctrl_message(_ctrl_message, argv);
    }

    bool validate_channel(const std::string &channel) {
        if (channel[0] != '&' && channel[0] != '#') {
            return false;
        }

        const std::vector<char> channel_forbidden_chars = { ' ', ',', '\a' };

        return all_of(channel_forbidden_chars.begin(), channel_forbidden_chars.end(), [&](const char &c){
            return channel.find(c) == std::string::npos;
        });
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

    void print_message(const std::string &message, const std::string &channel) {
        std::cout << "(" << channel << ") " <<  message << std::endl;
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