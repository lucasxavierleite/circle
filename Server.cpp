#include "Server.h"
#include "server_messages.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstring>
#include <cerrno>
#include <pthread.h>

namespace circle_server {
    pthread_t Server::tid;
    int Server::socket_fd;
    struct sockaddr_in Server::address;
    const char *Server::ip_address;
    int Server::port;
    pthread_mutex_t Server::clients_mutex;
    std::vector<User> Server::clients;
    bool Server::log;

    Server::Server() {
        Server::ip_address = DEFAULT_IP_ADDRESS;
        Server::port = DEFAULT_PORT;
        Server::clients_mutex = PTHREAD_MUTEX_INITIALIZER;
        Server::log = true;

        Server::socket_fd = socket(AF_INET, SOCK_STREAM, 0);

        int option = 1;
        if (setsockopt(Server::socket_fd, SOL_SOCKET, (SO_REUSEADDR | SO_REUSEPORT), &option, sizeof(option)) == -1) {
            print_error("Error setting socket options");
        }

        Server::address.sin_family = AF_INET;
        Server::address.sin_port = htons(Server::port);
        Server::address.sin_addr.s_addr = inet_addr(Server::ip_address);
    }

    void Server::set_port(char *&_port) {
        try {
            int _port_i = std::stoi(_port);
            Server::port = _port_i;
            Server::address.sin_port = htons(Server::port);
        } catch (std::exception &err) {
            errno = EINVAL;
            return print_error("Error setting port (" + (std::string) err.what() + ")");
        }
    }

    void Server::listen() {
        if (bind(socket_fd, (struct sockaddr *) &Server::address, sizeof(Server::address)) == -1) {
            print_error("Error binding the socket");
            close(Server::socket_fd);
            return;
        }

        std::cout << "Server " << Server::ip_address << " listening on port " << Server::port << std::endl;

        print_log(ctrl_message(SERVER_EMPTY));

        if (::listen(Server::socket_fd, MAX_CLIENTS_CONNECTED) == -1) {
            print_error("Error listening to clients");
            close(Server::socket_fd);
            return;
        }

        while (true) {
            struct sockaddr_in client_address{};
            socklen_t client_address_len = sizeof(client_address);
            int client_socket_fd = accept(Server::socket_fd, (struct sockaddr *) &client_address, &client_address_len);

            if (client_socket_fd == -1) {
                print_error("Error connecting to " + addrtos(client_address));
                break;
            }

            User client = User(client_socket_fd, client_address);
            add_client(client);
            pthread_create(&tid, nullptr, &handle_client, static_cast<void *>(&client));
            sleep(1);
        }
    }

    void *Server::handle_client(void *arg) {
        User client = *((User *) arg);

        send_response_message(client, ctrl_message(CLIENT_ENTER_NICKNAME));

        ssize_t bytes_received_nickname;
        char client_nickname[MESSAGE_MAX_CHARACTERS]{};

        while ((bytes_received_nickname = recv(client.get_socket_fd(), &client_nickname, sizeof(client_nickname), MSG_NOSIGNAL)) == -1) {
            print_error("Error receiving user nickname");
            send_response_message(client, ctrl_message(CLIENT_ENTER_NICKNAME));
        }

        if (bytes_received_nickname == 0) {
            close(client.get_socket_fd());
            remove_client(client);
            pthread_detach(pthread_self());
            return nullptr;
        }

        pthread_mutex_lock(&clients_mutex);
        clean_str(client_nickname);
        client.set_nickname(client_nickname);
        pthread_mutex_unlock(&clients_mutex);

        std::string join_message = ctrl_message(CLIENT_JOIN, client.get_nickname());
        print_message(join_message);
        send_message(client, join_message);

        bool client_quit = false;

        while (true) {
            char message_str[MESSAGE_MAX_CHARACTERS]{};
            ssize_t bytes_received;

            if ((bytes_received = recv(client.get_socket_fd(), &message_str, sizeof(message_str), MSG_NOSIGNAL)) == -1) {
                print_error("Error receiving the message");
                break;
            }

            clean_str(message_str);
            std::string message(message_str);

            if (bytes_received == 0) {
                message = ctrl_message(CLIENT_LEAVE, client.get_nickname());
                client_quit = true;
            } else {
                message = format_message(client.get_nickname(), message);
            }

            print_message(message);
            send_message(client, message);

            if (client_quit) {
                break;
            }
        }

        close(client.get_socket_fd());
        remove_client(client);

        pthread_detach(pthread_self());

        return nullptr;
    }

    void Server::add_client(User &client) {
        pthread_mutex_lock(&Server::clients_mutex);

        print_log("Adding client " + addrtos(client.get_address()) + " (" + std::to_string(client.get_socket_fd()) + ") to the list");
        Server::clients.push_back(client);

        pthread_mutex_unlock(&Server::clients_mutex);
    }

    void Server::remove_client(User &client) {
        pthread_mutex_lock(&Server::clients_mutex);

        print_log("Removing client " + client.get_nickname() + " from the list");

        if (clients.erase(std::find(clients.begin(), clients.end(), client)) != clients.end()) {
            print_log("User removed");
        }

        if (clients.empty()) {
            print_log(ctrl_message(SERVER_EMPTY));
        }

        pthread_mutex_unlock(&Server::clients_mutex);
    }

    void Server::send_message(User &client, const std::string &message) {
        pthread_mutex_lock(&Server::clients_mutex);

        for (auto &c : Server::clients) {
            if (c != client) {
                int attempts = 1;

                while (send(c.get_socket_fd(), message.c_str(), strlen(message.c_str()) * sizeof(char), MSG_NOSIGNAL) <= 0 && attempts < MAX_SEND_ATTEMPTS) {
                    print_error("Error sending message to " + c.get_nickname() + " (attempt " + std::to_string(attempts) + ")");
                    attempts++;
                }

                if (attempts == MAX_SEND_ATTEMPTS) {
                    print_error("Couldn't reach " + c.get_nickname() + " after " + std::to_string(attempts) + " attempts");
                    send_message(c, ctrl_message(CLIENT_LEAVE, c.get_nickname()));
                    close(c.get_socket_fd());
                    remove_client(c);
                    break;
                }
            }
        }

        pthread_mutex_unlock(&Server::clients_mutex);
    }

    void Server::send_response_message(User &client, const std::string &message) {
        pthread_mutex_lock(&Server::clients_mutex);

        int attempts = 1;

        while (send(client.get_socket_fd(), message.c_str(), strlen(message.c_str()) * sizeof(char), MSG_NOSIGNAL) <= 0 && attempts < MAX_SEND_ATTEMPTS) {
            print_error("Error sending message to " + client.get_nickname() + " (attempt " + std::to_string(attempts) + ")");
            attempts++;
        }

        if (attempts == MAX_SEND_ATTEMPTS) {
            print_error("Couldn't reach " + client.get_nickname() + " after " + std::to_string(attempts) + " attempts");
            send_message(client, ctrl_message(CLIENT_LEAVE, client.get_nickname()));
            close(client.get_socket_fd());
            remove_client(client);
        }

        pthread_mutex_unlock(&Server::clients_mutex);
    }

    void Server::print_log(const std::string &log_message) {
        if (Server::log) std::cout << "[LOG] " << log_message << std::endl;
    }
}