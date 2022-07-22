#include "Server.h"
#include <iostream>
#include <cstring>

int main(int argc, char *argv[]) {
    static circle_server::Server server;

    if (argc > 1 && strlen(argv[1]) > 0) circle_server::Server::set_port(argv[1]);

    circle_server::Server::listen();

    return 0;
}
