#ifndef CIRCLE_SERVER_MESSAGES_H
#define CIRCLE_SERVER_MESSAGES_H

#include <netinet/in.h>

#include <vector>
#include <string>

namespace circle_server {

    enum CTRL_MESSAGE {
        SERVER_EMPTY,
        CLIENT_WELCOME,
        CLIENT_ENTER_NICKNAME,
        CLIENT_JOIN,
        CLIENT_LEAVE,
        CLIENT_CHANGE_NICKNAME,
        CLIENT_INVALID_NICKNAME
    };

    std::string ctrl_message(enum CTRL_MESSAGE _ctrl_message, const std::string &arg = "");
    std::string ctrl_message(enum CTRL_MESSAGE _ctrl_message, std::vector<std::string> arg);
    std::string format_message(const std::string &nickname, const std::string &message);
    std::vector<std::string> split_message(const std::string &message);
    void print_message(const std::string &message);
    void print_error(const std::string &str);
    std::string addrtos(struct sockaddr_in address);
    void clean_str(char (&str)[]);

}

#endif