

#ifndef SERVER_SELECT_FUNCTIONS_H
#define SERVER_SELECT_FUNCTIONS_H
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
#include <unistd.h>
#include <signal.h>          /* signal */
#include <string.h>
#include <arpa/inet.h>
#include <inttypes.h>
#include <sys/ioctl.h>
#include "clients_list.h"
int sock;       /* The socket file descriptor for our "listening"socket */
int connectlist[5];  /* Array of connected sockets so we know who we are talking to */
fd_set socks;        /* Socket file descriptors we want to wakeup for, using select() */
int highsock;    /* Highest #'d file desc, needed for select() */

extern ListNode *clients_list;

void deal_with_data(int);


#endif //SERVER_SELECT_FUNCTIONS_H
