#ifndef CIRCLE_CLIENT_MESSAGES_H
#define CIRCLE_CLIENT_MESSAGES_H

#include <iostream>
#include <iomanip>
#include <string>
#include <map>
#include <cerrno>
#include <cstring>

namespace circle_client {

    enum INSTRUCTION {
        INVALID_COMMAND,
        QUIT,
        DEFAULT
    };

    std::string format_message(const std::string &nickname, const std::string &message);
    void print_message(const std::string &message);
    void print_instruction(enum INSTRUCTION instruction, const std::string &instruction_message = "");
    void print_error(const std::string &str);
    void print_welcome_message();
    void print_help();

}

#endif
