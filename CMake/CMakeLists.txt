cmake_minimum_required(VERSION 3.31)
project(FileDrop C)

set(CMAKE_C_STANDARD 11)

add_executable(tcp-server ../src/server/tcp_server.c
        ../include/utils.h
        ../include/doc_fork.h
        ../src/server/doc_fork_controller.c
        ../src/network_utils.c)

add_executable(tcp-client ../src/client/tcp_client.c
        ../include/client_doc_controller.h
        ../include/utils.h
        ../src/client/client_doc_handler.c
        ../src/network_utils.c)