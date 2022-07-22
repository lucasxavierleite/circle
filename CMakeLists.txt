cmake_minimum_required(VERSION 3.22)
project(circle)

set(CMAKE_CXX_STANDARD 20)
add_executable(circle_server circle_server.cpp  Server.cpp User.cpp server_messages.cpp nicknames.cpp)
add_executable(circle_client circle_client.cpp Client.cpp client_messages.cpp)