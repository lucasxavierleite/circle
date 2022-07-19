#include <iostream>
#include "Server.h"
#include <csignal>
#include <cstring>

int main(int argc, char *argv[]) {
//    signal(SIGPIPE, SIG_IGN);
//    signal(SIGINT, SIG_IGN);

    static circle::Server server;
    if (argc > 1 && strlen(argv[1]) > 0) circle::Server::set_port(argv[1]);
    circle::Server::listen();

    return 0;
}
