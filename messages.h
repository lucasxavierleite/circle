#ifndef CIRCLE_MESSAGES_H
#define CIRCLE_MESSAGES_H

#include <string>

enum CTRL_MESSAGE {
    CLIENT_ENTER_NICKNAME,
    CLIENT_JOIN,
    CLIENT_LEAVE
};

std::string ctrl_message(enum CTRL_MESSAGE ctrl_message, const std::string &arg = "");
std::string format_message(const std::string &nickname, const std::string &message);
void clean_str(char (&str)[]);
void print_error(const std::string &str);
std::string addrtos(struct sockaddr_in address);

#endif