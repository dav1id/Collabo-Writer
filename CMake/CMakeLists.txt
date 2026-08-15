cmake_minimum_required(VERSION 3.31)
project(FileDrop C)

set(CMAKE_C_STANDARD 11)

add_executable(tcp-server ../src/server/tcp_server.c
        ../include/client_doc_controller.h
        ../include/utils.h
        ../src/client/client_doc_handler.c
        ../include/doc_fork.h
        ../src/server/doc_fork_controller.c
        ../src/network_utils.c
        ../src/config/dummy_client.c)

add_executable(dummy_server ../src/config/dummy_server.c
        ../../include/utils.h)

add_executable(dummy_client ../src/config/dummy_client.c
                ../../include/utils.h)
