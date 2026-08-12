cmake_minimum_required(VERSION 3.31)
project(FileDrop C)

set(CMAKE_C_STANDARD 11)

add_executable(tcp-server ../src/tcp-server.c
        ../src/ncurses.c)

