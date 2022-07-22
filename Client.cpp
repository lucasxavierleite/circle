#include "Client.h"
#include "client_messages.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <string>
#include <cstring>
#include <iostream>

namespace circle_client {

    Client::Client() {
        connected = false;
        nickname = {};
        recv_thread = {};
        mutex = PTHREAD_MUTEX_INITIALIZER;
    }

    int Client::get_socket_fd() const {
        return socket_fd;
    }

    std::string Client::get_nickname() {
        return nickname;
    }

    void Client::set_nickname(std::string &_nickname) {
        nickname = _nickname;
    }

    void Client::connect() {
        struct sockaddr_in server_address{};

        std::string server_ip(SERVER_IP_ADDRESS);
        int server_port = SERVER_PORT;

        server_address.sin_family = AF_INET;
        server_address.sin_port = htons(server_port);
        server_address.sin_addr.s_addr = inet_addr(server_ip.c_str());

        socket_fd = socket(AF_INET, SOCK_STREAM, 0);

        if (::connect(socket_fd, (struct sockaddr *) &server_address, sizeof(server_address)) == -1) {
            throw std::runtime_error("Error connecting to server " + server_ip + ":" + std::to_string(server_port));
        }

        print_instruction(DEFAULT, "Connected to server " + server_ip + ":" + std::to_string(server_port));

        connected = true;
    }

    int Client::init() {
        pthread_create(&recv_thread, nullptr, &handle_recv, static_cast<void *>(this));

        std::locale::global(std::locale(""));

        std::string message;

        while (std::getline(std::cin, message)) {
            if (!message.empty()) {

                if (message == "/quit") {
                    break;
                }

                ssize_t bytes_sent;

                if ((bytes_sent = send(socket_fd, message.c_str(), strlen(message.c_str()) * sizeof(char),MSG_NOSIGNAL)) == -1) {
                    disconnect();
                    throw std::runtime_error("Error sending message");
                }

                if (bytes_sent == 0 || !connected) {
                    break;
                }

                message = {};
            }
        }

        disconnect();
        return 0;
    }

    void *handle_recv(void *arg) {
        Client client = *((Client *) arg);

        while (client.is_connected()) {
            char message_str[MESSAGE_MAX_CHARACTERS]{};
            ssize_t bytes_received;

            if ((bytes_received = recv(client.get_socket_fd(), &message_str, MESSAGE_MAX_CHARACTERS, MSG_NOSIGNAL)) == -1) {
                if (client.is_connected()) {
                    print_error("Error receiving message");
                }

                break;
            }

            if (bytes_received == 0 || !client.is_connected()) {
                break;
            }

            std::string message(message_str);
            print_message(message);
        }

        client.disconnect();
        pthread_detach(pthread_self());
        return nullptr;
    }

    void Client::disconnect() {
        if (!connected) {
            return;
        }

        pthread_mutex_lock(&mutex);

        connected = false;
        close(socket_fd);

        pthread_mutex_unlock(&mutex);
    }

    bool Client::is_connected() const {
        return connected;
    }

}