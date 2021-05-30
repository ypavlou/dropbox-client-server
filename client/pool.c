#include "pool.h"

void initialize(pool_t * pool) {

    pool->ipAddr = malloc(POOL_SIZE * sizeof(char *));    //allocate space for an array of strings
    for(int i=0; i<POOL_SIZE; i++){
        (pool->ipAddr)[i] = malloc(256 * sizeof(char));
    }

    pool->portNum = malloc(POOL_SIZE * sizeof(uint16_t));      //allocate space for an array of uint16_t type


    pool->pathname = malloc(POOL_SIZE * sizeof(char *));
    for(int i=0; i<POOL_SIZE; i++){
        (pool->pathname)[i] = malloc(260 * sizeof(char));
    }
    pool->version = malloc(POOL_SIZE * sizeof(time_t));

    for(int i = 0; i < POOL_SIZE; i++){
        strcpy((pool->ipAddr)[i],"empty");
        strcpy((pool->pathname)[i],"empty");
        (pool->portNum)[i] = 0;
        (pool->version)[i] = 0;
    }

    pool->start = 0;
    pool->end = -1;
    pool->count = 0;
}

void add_2(pool_t * pool, char *ip ,uint16_t port) {
    pthread_mutex_lock(&pool_mtx);
    while (pool->count >= POOL_SIZE) {
        printf(">> Found Buffer Full \n");
        pthread_cond_wait(&pool_cond_nonfull, &pool_mtx);       //wait for signal from another thread
    }
    pool->end = (pool->end + 1) % POOL_SIZE;            //find the next available slot

    pool->portNum[pool->end] = port;            //copy id and port to buffers
    strcpy(pool->ipAddr[pool->end],ip);

    pool->count++;
    pthread_mutex_unlock(&pool_mtx);
}

void add_4(pool_t * pool, char *ip ,uint16_t port, char *path, time_t version){
    pthread_mutex_lock(&pool_mtx);
    while (pool->count >= POOL_SIZE) {
        printf(">> Found Buffer Full \n");
        pthread_cond_wait(&pool_cond_nonfull, &pool_mtx);       //wait for signal from another thread
    }
    pool->end = (pool->end + 1) % POOL_SIZE;            //find the next available slot

    pool->portNum[pool->end] = port;            //copy id , port,path and version to buffers
    strcpy(pool->ipAddr[pool->end],ip);
    strcpy(pool->pathname[pool->end],path);
    pool->version[pool->end] = version;

    pool->count++;
    pthread_mutex_unlock(&pool_mtx);
}
void print_p(pool_t * pool){
    printf("\n");
    for(int i = 0; i < POOL_SIZE; i++){
        printf("IP: %s, PORT: %u, PATH: %s, VERSION: %ld \n",(pool->ipAddr)[i], (pool->portNum)[i], (pool->pathname)[i], (pool->version)[i] );
    }
}

int check_data(pool_t * pool) {

    pthread_mutex_lock(&pool_mtx);
    while (pool->count <= 0) {
        printf(">> Found Buffer Empty \n");
        pthread_cond_wait(&pool_cond_nonempty, &pool_mtx);
    }
    if((strcmp(pool->pathname[pool->start], "empty") == 0) && pool->version[pool->start] == 0  ){
        pthread_mutex_unlock(&pool_mtx);
        return 2;
    }else{
        pthread_mutex_unlock(&pool_mtx);
        return 4;
    }
}

void delete_pool(pool_t *pool){
    for(int i =0; i < POOL_SIZE; i++){
        free(pool->ipAddr[i]);

    }free(pool->ipAddr);

    for(int i =0; i < POOL_SIZE; i++){
        free(pool->pathname[i]);

    }free(pool->pathname);

    free(pool->version);
    free(pool->portNum);

    pool = NULL;



}