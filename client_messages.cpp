#include "client_messages.h"

#include <iostream>
#include <iomanip>
#include <string>
#include <map>
#include <cerrno>
#include <cstring>
#include <sys/ioctl.h>
#include <unistd.h>

namespace circle_client {

    std::map<std::string, std::string> cmd_desc;

    void build_cmd_desc() {
        cmd_desc["/connect"] = "connect to server";
        cmd_desc["/help"] = "print help instructions";
        cmd_desc["/quit"] = "close the chat";
    }

    void print_instruction(enum INSTRUCTION instruction, const std::string &instruction_message) {
        std::string output;

        switch (instruction) {
            case INVALID_COMMAND:
                output = "Invalid command. Type /help to see the available commands";
                break;
            case QUIT:
                output = "Type /quit to close the chat";
                break;
            default:
                output = instruction_message;
                break;
        }

        std::cout << output << std::endl;
    }

    void print_welcome_message() {
        std::cout << "Welcome to Circle IRC" << std::endl << std::endl;
        print_help();
    }

    void print_help() {
        if (cmd_desc.empty()) {
            build_cmd_desc();
        }

        struct winsize w{};
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        std::ostringstream help_message("Available commands:\n", std::ios::ate);
        help_message << std::setfill('-') << std::setw(w.ws_col > 0 ? w.ws_col : 80) << " " << std::setfill(' ')
                     << std::endl;

        for (auto &c: cmd_desc) {
            help_message << std::left << std::setw(24) << c.first << c.second << std::endl;
        }

        std::cout << help_message.str() << std::endl;
    }

    std::string format_message(const std::string &nickname, const std::string &message) {
        return nickname + ": " + message;
    }

    void print_message(const std::string &message) {
        std::cout << message << std::endl;
    }

    void print_error(const std::string &str) {
        std::cout << "[ERROR " << errno << "] " << str << ": " << std::strerror(errno) << std::endl;
    }

}