cmake_minimum_required(VERSION 3.14)
project(client C)

set(CMAKE_C_STANDARD 99)

add_executable(client dropbox_client.c server_connect.c server_connect.h client_list.c client_list.h client_server.c client_server.h files_list.c files_list.h pool.c pool.h worker_threads.c worker_threads.h)