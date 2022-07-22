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

    std::cout << "> ";
    std::cout.flush();

    while (std::cin >> command) {
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
        } else if (command == "/quit") {
            break;
        } else {
            print_instruction(circle_client::INVALID_COMMAND);
        }

        std::cout << "> ";
        std::cout.flush();
    }

    std::cout << std::endl;
    return 0;
}