cmake_minimum_required(VERSION 3.31)
project(FileDrop C)

set(CMAKE_C_STANDARD 11)

add_executable(tcp-server ../src/server/tcp-server.c
        ../src/ncurses.c
        ../include/client_doc_writer.h
        ../include/utils.h
        ../include/network.h
        ../include/server_doc_writer.h
        ../src/client/client_doc_handler.c)

