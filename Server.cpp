#include "Server.h"
#include "server_messages.h"
#include "nicknames.h"

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
    std::map<std::string, std::vector<User>> Server::channels;
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

        print_log("Server " + std::string(Server::ip_address) + " listening on port " + std::to_string(Server::port));
        print_log("Waiting for new clients");

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
            pthread_create(&tid, nullptr, &handle_client, static_cast<void *>(&client));
            sleep(1);
        }
    }

    void *Server::handle_client(void *arg) {
        User client = *((User *) arg);

        std::string client_nickname = get_random_nickname();
        client.set_nickname(client_nickname);

        send_response_message(client, ctrl_message(WELCOME, client.get_nickname()));

        while (!client.is_kicked()) {
            char message_str[MESSAGE_MAX_CHARACTERS]{};
            ssize_t bytes_received;

            if ((bytes_received = recv(client.get_socket_fd(), &message_str, MESSAGE_MAX_CHARACTERS - client.get_nickname().length() - 2, MSG_NOSIGNAL)) == -1) {
                print_error("Error receiving the message");
                break;
            }

            client = update_client(client);

            clean_str(message_str);
            std::string message(message_str);

            if (bytes_received == 0 || client.is_kicked()) {
                if (!client.get_channel().empty()) {
                    message = ctrl_message(LEAVE, { client.get_nickname(), client.get_channel() });
                    send_message(client, message);
                }

                break;
            } else {
                std::vector<std::string> split = split_message(message);
                std::string command = split[0];
                std::string argument = split.size() == 2 ? split[1] : "";

                if (command == "/nickname") {
                    if (argument.empty()) {
                        message = ctrl_message(PROVIDE_ARGUMENT);
                        send_response_message(client, message);
                    } else if (validate_nickname(argument)) {
                        std::string old_nickname = client.get_nickname();
                        client.set_nickname(argument);
                        message = ctrl_message(CHANGE_NICKNAME, { old_nickname, client.get_nickname() });
                        send_message(client, message);
                    } else {
                        message = ctrl_message(INVALID_NICKNAME);
                        send_response_message(client, message);
                        continue;
                    }
                } else if(command == "/ping") {
                    message = ctrl_message(PING);
                    send_response_message(client, message);
                    continue;
                } else if (command == "/join") {
                    if (argument.empty()) {
                        message = ctrl_message(PROVIDE_ARGUMENT);
                        send_response_message(client, message);
                    } else if (argument == client.get_channel()) {
                        message = ctrl_message(JOIN_CURRENT_CHANNEL, argument);
                        send_response_message(client, message);
                    } else if (validate_channel(argument)) {
                        if (!client.get_channel().empty()) {
                            remove_client(client);
                            message = ctrl_message(LEAVE, { client.get_nickname(), client.get_channel() });
                            send_message(client, message);
                        }

                        bool new_admin = (Server::channels.find(argument) == Server::channels.end());

                        message = ctrl_message(JOIN, { client.get_nickname(), argument });
                        client.set_channel(argument);
                        add_client(client);
                        send_message(client, message);

                        if (new_admin) {
                            send_response_message(client, ctrl_message(NEW_ADMIN));
                        }
                    } else {
                        message = ctrl_message(INVALID_CHANNEL_NAME);
                        send_response_message(client, message);
                        continue;
                    }
                } else if (command == "/kick") {
                    if (client.is_admin()) {
                        if (argument.empty()) {
                            message = ctrl_message(PROVIDE_ARGUMENT);
                            send_response_message(client, message);
                        } else if (argument == client.get_nickname()) {
                            message = ctrl_message(SELF_KICK);
                            send_response_message(client, message);
                        } else if (!kick_client(client, argument)) {
                            message = ctrl_message(USER_NOT_FOUND, argument);
                            send_response_message(client, message);
                        } else {
                            message = ctrl_message(KICK, argument);
                            send_message(client, message);
                        }
                    } else {
                        message = ctrl_message(NOT_ADMIN);
                        send_response_message(client, message);
                    }
                } else if ((command == "/mute" || command == "/unmute") && !client.get_channel().empty()) {
                    if (client.is_admin()) {
                        if (argument.empty()) {
                            message = ctrl_message(PROVIDE_ARGUMENT);
                            send_response_message(client, message);
                        } else if (!set_mute_client(client, argument, (command == "/mute"))) {
                            message = ctrl_message(USER_NOT_FOUND, argument);
                            send_response_message(client, message);
                        }
                    } else {
                        message = ctrl_message(NOT_ADMIN);
                        send_response_message(client, message);
                    }
                } else if (!client.get_channel().empty()) {
                    if (!client.is_mute()) {
                        message = format_message(client.get_nickname(), message);
                        print_message(message, client.get_channel());
                        send_message(client, message);
                    } else {
                        message = ctrl_message(MUTE);
                        send_response_message(client, message);
                    }
                } else {
                    message = ctrl_message(NO_CHANNEL);
                    send_response_message(client, message);
                }
            }
        }

        close(client.get_socket_fd());

        if (!client.get_channel().empty()) {
            remove_client(client);
        }

        pthread_detach(pthread_self());

        return nullptr;
    }

    User Server::update_client(User &client) {
        if (!client.get_channel().empty()) {
            auto channel = Server::channels.find(client.get_channel());
            if (channel != Server::channels.end()) {
                auto channel_users = channel->second;
                return *(std::find(channel_users.begin(), channel_users.end(), client));
            }
        }

        return client;
    }

    void Server::add_client(User &client) {
        if (client.get_channel().empty()) {
            return;
        }

        auto channel = Server::channels.find(client.get_channel());

        pthread_mutex_lock(&Server::clients_mutex);

        print_log("Adding client " + addrtos(client.get_address()) + " (" + std::to_string(client.get_socket_fd()) + ") to channel " + client.get_channel());

        if (channel == Server::channels.end()) {
            print_log("Channel doesn't exist. Creating one");
            client.set_admin(true);
            std::vector<User> users{};
            users.push_back(client);
            Server::channels.insert(std::pair<std::string, std::vector<User>>(client.get_channel(), users));
        } else {
            print_log("Channel already exists. Adding user to the list");
            client.set_admin(false);
            channel->second.push_back(client);
        }

        pthread_mutex_unlock(&Server::clients_mutex);
    }

    void Server::remove_client(User &client) {
        if (client.get_channel().empty()) {
            return;
        }

        print_log("Removing client " + client.get_nickname() + " from channel " + client.get_channel());

        auto channel = Server::channels.find(client.get_channel());

        if (channel == Server::channels.end()) {
            return;
        }

        std::vector<User> channel_users = channel->second;

        pthread_mutex_lock(&Server::clients_mutex);

        if (channel_users.erase(std::find(channel_users.begin(), channel_users.end(), client)) != channel_users.end()) {
            channel->second = channel_users;
            client.set_admin(false);
            print_log("User removed");
        }

        pthread_mutex_unlock(&Server::clients_mutex);
    }

    bool Server::kick_client(User &admin, const std::string &nickname) {
        if (admin.get_channel().empty()) {
            return false;
        }

        auto channel = Server::channels.find(admin.get_channel());

        if (channel == Server::channels.end()) {
            return false;
        }

        std::vector<User> channel_users = channel->second;
        auto user = std::find_if(channel_users.begin(), channel_users.end(), [&](User &u) {
            return u.get_nickname() == nickname;
        });

        if (user == channel_users.end()) {
            return false;
        }

        pthread_mutex_lock(&clients_mutex);

        (*user).set_kicked(true);
        channel->second = channel_users;

        pthread_mutex_unlock(&clients_mutex);

        remove_client(*user);

        return true;
    }

    bool Server::set_mute_client(User &admin, const std::string &nickname, const bool &mute) {
        if (admin.get_channel().empty()) {
            return false;
        }

        auto channel = Server::channels.find(admin.get_channel());

        if (channel == Server::channels.end()) {
            return false;
        }

        std::vector<User> channel_users = channel->second;
        auto user = std::find_if(channel_users.begin(), channel_users.end(), [&](User &u) {
           return u.get_nickname() == nickname;
        });

        if (user == channel_users.end()) {
            return false;
        }

        pthread_mutex_lock(&Server::clients_mutex);

        (*user).set_mute(mute);
        channel->second = channel_users;

        pthread_mutex_unlock(&Server::clients_mutex);

        return true;
    }

    void Server::send_message(User &client, const std::string &message) {
        if (client.get_channel().empty()) {
            return;
        }

        auto channel = Server::channels.find(client.get_channel());

        if (channel == Server::channels.end()) {
            return;
        }

        std::vector<User> clients = channel->second;

        pthread_mutex_lock(&Server::clients_mutex);

        for (auto &c : clients) {
            int attempts = 1;

            while (send(c.get_socket_fd(), message.c_str(), strlen(message.c_str()) * sizeof(char), MSG_NOSIGNAL) <= 0 && attempts <= MAX_SEND_ATTEMPTS) {
                print_error("Error sending message to " + c.get_nickname() + " (attempt " + std::to_string(attempts) + ")");
                attempts++;
            }

            if (attempts >= MAX_SEND_ATTEMPTS) {
                print_error("Couldn't reach " + c.get_nickname() + " after " + std::to_string(attempts - 1) + " attempts");
//                send_message(c, ctrl_message(LEAVE, c.get_nickname()));
                close(c.get_socket_fd());
                remove_client(c);
                break;
            }
        }

        pthread_mutex_unlock(&Server::clients_mutex);
    }

    void Server::send_response_message(User &client, const std::string &message) {
        pthread_mutex_lock(&Server::clients_mutex);

        int attempts = 1;

        while (send(client.get_socket_fd(), message.c_str(), strlen(message.c_str()) * sizeof(char), MSG_NOSIGNAL) <= 0 && attempts <= MAX_SEND_ATTEMPTS) {
            print_error("Error sending message to " + client.get_nickname() + " (attempt " + std::to_string(attempts) + ")");
            attempts++;
        }

        if (attempts >= MAX_SEND_ATTEMPTS) {
            print_error("Couldn't reach " + client.get_nickname() + " after " + std::to_string(attempts - 1) + " attempts");
//            send_message(client, ctrl_message(LEAVE, client.get_nickname()));
            close(client.get_socket_fd());
            remove_client(client);
        }

        pthread_mutex_unlock(&Server::clients_mutex);
    }

    void Server::print_log(const std::string &log_message) {
        if (Server::log) std::cout << "[LOG] " << log_message << std::endl;
    }
}