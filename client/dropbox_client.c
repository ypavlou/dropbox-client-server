#include <pthread.h>
#include "server_connect.h"
#include "client_list.h"
#include "client_server.h"
#include "worker_threads.h"

#define MAX_PATHNAME_LEN 260
ListNode *clients_list=NULL;
pthread_mutex_t mtx;    //for client list
int sock;       /* The socket file descriptor for our "listening"socket */
int connectlist[5];  /* Array of connected sockets so we know who we are talking to */
fd_set socks;        /* Socket file descriptors we want to wakeup for, using select() */
int highsock;    /* Highest #'d file desc, needed for select() */
int POOL_SIZE = 0 ;
char dirName[MAX_PATHNAME_LEN];
int workerThreads;

pthread_mutex_t pool_mtx;   //for thread pool
pthread_cond_t pool_cond_nonempty;
pthread_cond_t pool_cond_nonfull;
pool_t pool;

 int catch = 0;
void sigint_handler(int);       //signal handler for sigint


int main(int argc, char *argv[]) {

    char serverIp[256];
    int    server_port = 0, my_port = 0,client_sock,bufferSize;
    struct sockaddr_in server;
    struct sockaddr *serverptr = (struct sockaddr*)&server;
    struct hostent *rem;
    if(argc > 13){
        printf("Command Execution Failed\n");
        exit(1);
    }
    else if(argc == 13){
        for(int i=1; i< argc; i+=2) {//read input from command line
            if (strcmp(argv[i], "-d") == 0) {
                strcpy(dirName,argv[i+1]);

            } else if (strcmp(argv[i], "-p") == 0) {
                my_port = atoi(argv[i+1]);

            } else if (strcmp(argv[i], "-w") == 0) {
                workerThreads = atoi(argv[i+1]);

            } else if (strcmp(argv[i], "-b") == 0) {
                bufferSize = atoi(argv[i+1]);
                POOL_SIZE = bufferSize;

            } else if (strcmp(argv[i], "-sp") == 0) {
                server_port = atoi(argv[i+1]);
            } else if (strcmp(argv[i], "-sip") == 0) {
                strcpy(serverIp,argv[i+1]);
            }
        }
    }
    else{
        printf("Command Execution Failed\n");
        exit(-1);
    }

    struct sigaction sa;

    sigfillset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = sigint_handler; // Setup the sigint handler

    if(sigaction(SIGINT, &sa, NULL) == -1){
        perror("sigaction");
        exit(EXIT_FAILURE);
    }


    if(pthread_mutex_init(&mtx, NULL) != 0){ //initialize mutex
        printf("error: mutex init\n");
        exit(EXIT_FAILURE);
    }

    //pthread_t my_thread;
    initialize(&pool);                              //initialize buffer
    if(pthread_mutex_init(&pool_mtx, NULL) != 0){
        printf("error: mutex init\n");
        exit(EXIT_FAILURE);
    }
    if(pthread_cond_init(&pool_cond_nonempty, 0) != 0){      //initialize mutex and condition variables for buffer
        printf("error: cond init\n");
        exit(EXIT_FAILURE);
    }
    if(pthread_cond_init(&pool_cond_nonfull, 0) != 0){
        printf("error: cond init\n");
        exit(EXIT_FAILURE);
    }

    if ((client_sock = socket(PF_INET, SOCK_STREAM, 0)) < 0)   //create socket fd for connection to server
        perror_exit("socket");

    if ((rem = gethostbyname(serverIp)) == NULL) {      //find server address
        herror("gethostbyname");
        exit(1);
    }
    server.sin_family = AF_INET;        //internet domain
    memcpy(&server.sin_addr, rem->h_addr, rem->h_length);
    server.sin_port = htons(server_port);         //server port

    if (connect(client_sock, serverptr, sizeof(server)) < 0)   //connection
        perror_exit("connect");
    printf("Connecting to server: %s port %d\n", serverIp, server_port);
    connect_to_server(client_sock,server_port,my_port); //send LOG_ON and GET_CLIENTS to server

    if(catch ==1){

        exit(EXIT_SUCCESS);
    }

    create_client_server(my_port,dirName,client_sock);


    //received sigint
    log_off(client_sock,my_port);
    deleteALL();


    close(client_sock);
}

void sigint_handler(int sig){
    catch=1;
}

