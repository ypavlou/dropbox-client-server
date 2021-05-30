#ifndef CLIENT_POOL_H
#define CLIENT_POOL_H

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>	     //uint16_t
#include <string.h>

extern int POOL_SIZE;
extern pthread_mutex_t pool_mtx;
extern pthread_cond_t pool_cond_nonempty;
extern pthread_cond_t pool_cond_nonfull;

typedef struct {
    char **ipAddr;
    uint16_t *portNum;
    char **pathname;
    time_t *version;

    int start;
    int end;
    int count;
} pool_t;
void initialize(pool_t *);
void add_2(pool_t *, char * ,uint16_t );
void print_p(pool_t * );
int check_data(pool_t * );
void add_4(pool_t * , char * ,uint16_t, char *, time_t );
void delete_pool(pool_t *);
#endif //CLIENT_POOL_H
