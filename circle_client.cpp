#include "Client.h"
#include "client_messages.h"

#include <iostream>
#include <csignal>

void handler(int signal) {
    std::cout << std::endl;
    circle_client::print_instruction(circle_client::QUIT);
    std::cin.clear();
    std::cout << "> ";
    std::cout.flush();
}

int main(int argc, char *argv[]) {
    signal(SIGINT, handler);

    circle_client::print_welcome_message();

    circle_client::Client client;

    std::string command;

    while (true) {
        std::cout << "> ";
        std::cout.flush();

        std::cin >> command;

        if (command == "/connect") {
            try {
                client.connect();
                return client.init();
            } catch (std::exception &err) {
                circle_client::print_error(err.what());
                continue;
            }
        } else if (command == "/help") {
            circle_client::print_help();
        } else if (command == "/quit" || std::cin.eof()) {
            std::cout << std::endl;
            return 0;
        } else {
            print_instruction(circle_client::INVALID_COMMAND);
        }
    }
}