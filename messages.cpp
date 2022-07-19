#include "messages.h"

#include <netinet/in.h>

#include <iostream>
#include <cerrno>
#include <cstring>
#include <string>

void print_error(const std::string &str) {
    std::cout << "[ERROR " << errno << "] " << str << ": " << std::strerror(errno) << std::endl;
}

std::string ctrl_message(enum CTRL_MESSAGE ctrl_message, const std::string &arg) {
    std::string message;

    switch (ctrl_message) {
        case CLIENT_ENTER_NICKNAME:
            message = "Please, enter your nickname: ";
            break;
        case CLIENT_JOIN:
            message = arg + " joined the chat";
            break;
        case CLIENT_LEAVE:
            message = arg + " left the chat";
            break;
    }

    return message;
}

std::string format_message(const std::string &nickname, const std::string &message) {
    return nickname + ": " + message;
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