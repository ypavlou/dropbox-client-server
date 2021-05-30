cmake_minimum_required(VERSION 3.14)
project(server C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(dropbox_server
        clients_list.c
        clients_list.h
        dropbox_server.h
        dropbox_server.c select_functions.c select_functions.h)
