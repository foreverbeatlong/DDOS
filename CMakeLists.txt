cmake_minimum_required(VERSION 3.17)
project(DDOS)

set(CMAKE_CXX_STANDARD 14)
add_executable(DDOS main.cpp)
add_executable(client tcpclient.c)
add_executable(server tcpserver.c)

