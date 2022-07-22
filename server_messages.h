#ifndef CIRCLE_SERVER_MESSAGES_H
#define CIRCLE_SERVER_MESSAGES_H

#include <netinet/in.h>

#include <vector>
#include <string>

namespace circle_server {

    enum CTRL_MESSAGE {
        WELCOME,
        JOIN,
        LEAVE,
        CHANGE_NICKNAME,
        INVALID_NICKNAME,
        PING,
        INVALID_CHANNEL_NAME,
        NO_CHANNEL
    };

    std::string ctrl_message(enum CTRL_MESSAGE _ctrl_message, const std::string &arg = "");
    std::string ctrl_message(enum CTRL_MESSAGE _ctrl_message, std::vector<std::string> arg);
    std::string format_message(const std::string &nickname, const std::string &message);
    std::vector<std::string> split_message(const std::string &message);
    bool validate_channel(const std::string &channel);
    void print_message(const std::string &message, const std::string &channel);
    void print_error(const std::string &str);
    std::string addrtos(struct sockaddr_in address);
    void clean_str(char (&str)[]);

}

#endif