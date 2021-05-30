#ifndef CLIENT_WORKER_THREADS_H
#define CLIENT_WORKER_THREADS_H
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <libgen.h>
#include "pool.h"
#include "client_list.h"

#define MAX_PATHNAME_LEN 260

extern ListNode *clients_list;
extern pthread_mutex_t mtx;    //for client list
extern pool_t pool;
extern char dirName[MAX_PATHNAME_LEN];

void * work(void *);
void connect_to_peer_2(char *, uint16_t);
void send_get_file_list(int sock, char*, uint16_t);
void get_tuple_2(pool_t *);
void get_tuple_4(pool_t *);
void connect_to_peer_4(char *, uint16_t , char *, time_t,char* );
void send_get_file(int , char *, uint16_t, char*, time_t,char* );
void read_file(int , int , char *);
#endif //CLIENT_WORKER_THREADS_H
