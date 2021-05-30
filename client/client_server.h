#ifndef CLIENT_CLIENT_SERVER_H
#define CLIENT_CLIENT_SERVER_H
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/time.h>
#include <fcntl.h>
#include <sys/wait.h>	     /* sockets */
#include <sys/types.h>	     /* sockets */
#include <sys/socket.h>	     /* sockets */
#include <netinet/in.h>	     /* internet sockets */
#include <netdb.h>	         /* gethostbyaddr */
#include <string.h>
#include <arpa/inet.h>
#include <inttypes.h>
#include <sys/ioctl.h>
#include <dirent.h>
#include <libgen.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include "files_list.h"
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include "client_list.h"
#include "worker_threads.h"


#define MAX_PATHNAME_LEN 260
extern int sock;
extern int connectlist[5];
extern fd_set socks;
extern int highsock;
extern int workerThreads;
extern ListNode *clients_list;
extern pthread_mutex_t mtx;
extern int catch;


void build_select_list();
void handle_new_connection();
void read_socks(char *);
void create_client_server(int,char *,int);
void deal_with_data(int ,char *);
void deleteALL();
#endif //CLIENT_CLIENT_SERVER_H
