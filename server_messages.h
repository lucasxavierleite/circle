#ifndef CIRCLE_SERVER_MESSAGES_H
#define CIRCLE_SERVER_MESSAGES_H

#include <netinet/in.h>

#include <string>

namespace circle_server {

    enum CTRL_MESSAGE {
        SERVER_EMPTY,
        CLIENT_ENTER_NICKNAME,
        CLIENT_JOIN,
        CLIENT_LEAVE
    };

    std::string ctrl_message(enum CTRL_MESSAGE ctrl_message, const std::string &arg = "");
    std::string format_message(const std::string &nickname, const std::string &message);
    void print_message(const std::string &message);
    void print_error(const std::string &str);
    std::string addrtos(struct sockaddr_in address);
    void clean_str(char (&str)[]);

}

#endif