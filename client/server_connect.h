#ifndef CLIENT_SERVER_CONNECT_H
#define CLIENT_SERVER_CONNECT_H
#include <stdio.h>
#include <sys/types.h>/* sockets */
#include <sys/socket.h>/* sockets */
#include <netinet/in.h>/* internet sockets */
#include <unistd.h>          /* read, write, close */
#include <netdb.h>/* gethostbyaddr */
#include <stdlib.h>/* exit */
#include <string.h>/* strlen */
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "client_list.h"
#include "worker_threads.h"


void connect_to_server(int, int, int);
void perror_exit(char *);
void add_in_list(char *, uint16_t);
void log_off(int , int );
void get_files(char *, int);
#endif //CLIENT_SERVER_CONNECT_H
