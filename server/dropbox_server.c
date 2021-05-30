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
#include "select_functions.h"
#include "clients_list.h"



ListNode *clients_list=NULL;
int catch = 0;
void sigint_handler(int);       //signal handler for sigint

void setnonblocking(soock)int soock;{
    int opts;
    opts = fcntl(soock, F_GETFL);
    if (opts < 0) {
        perror("fcntl(F_GETFL)");
        exit(EXIT_FAILURE);
    }
    opts = (opts | O_NONBLOCK);
    if (fcntl(soock, F_SETFL, opts) < 0) {
        perror("fcntl(F_SETFL)");
        exit(EXIT_FAILURE);
    }
}

void build_select_list() {
    int listnum;/* Current item in connectlist for for loops */
    /* First put together fd_set for select(), which willconsist of the sock veriable in case a new connectionis coming in,
     * plus all the sockets we have alreadyaccepted. */
    /* FD_ZERO() clears out the fd_set called socks, so thatit doesn't contain any file descriptors. */
    FD_ZERO(&socks);
    /* FD_SET() adds the file descriptor "sock" to the fd_set,so that select() will return if a connection comes in onthat socket
     * (which means you have to accept(), etc. */
    FD_SET(sock,&socks);
    /* Loops through all the possible connections and addsthose sockets to the fd_set */
    for (listnum = 0;listnum <5; listnum++) {
        if (connectlist[listnum] != 0) {
            FD_SET(connectlist[listnum], &socks);
            if (connectlist[listnum] > highsock)
                highsock = connectlist[listnum];
        }
    }
}
void handle_new_connection() {
    int listnum;/* Current item in connectlist for for loops */
    int connection; /* Socket file descriptor for incoming connections */
    /* We have a new connection coming in!  We'lltry to find a spot for it in connectlist. */
    struct sockaddr_in client_connected;
    socklen_t addr_size;
    addr_size = sizeof(client_connected);

    connection = accept(sock, (struct sockaddr*) &client_connected, &addr_size);
    if (connection < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(client_connected.sin_addr), ip, INET_ADDRSTRLEN);

    // for finding port number of client
    printf("connection established with IP : %s and PORT : %d\n", ip, ntohs(client_connected.sin_port));
    setnonblocking(connection);
    for (listnum = 0; (listnum < 5) && (connection != -1); listnum++)
        if (connectlist[listnum] == 0) {
            printf("\nConnection accepted:   FD=%d; Slot=%d\n", connection, listnum);
            connectlist[listnum] = connection;
            connection = -1;
        }
    if (connection != -1) {/* No room left in the queue! */
        printf("\nNo room left for new client.\n");
        //sock_puts(connection, "Sorry, this server is too busy.Try again later!\r\n");
        close(connection);
    }
}

void read_socks() {
    int listnum;
    /* Current item in connectlist for for loops */
    /* OK, now socks will be set with whatever socket(s)are ready for reading.
 * Lets first check our"listening" socket, and then check the socketsin connectlist. */
    /* If a client is trying to connect() to our listening socket, select() will consider that as the socketbeing 'readable'.
     * Thus, if the listening socket ispart of the fd_set, we need to accept a new connection. */
    if (FD_ISSET(sock, &socks))
        handle_new_connection();
    /* Now check connectlist for available data */
    /* Run through our sockets and check to see if anythinghappened with them, if so 'service' them. */
    for (listnum = 0;listnum <5; listnum++) {
        if (FD_ISSET(connectlist[listnum], &socks)) {
            deal_with_data(listnum);
        }
    } /* for (all entries in queue) */
}



int main (argc, argv) int argc;char *argv[];{
    char *ascport;
    int port;
    struct sockaddr_in server_address;
    int reuse_addr = 1;  /* Used so we can re-bind to our portwhile a previous connection is stillin TIME_WAIT state. */
    int readsocks;

    if(argc > 3){
        printf("Command Execution Failed\n");
        exit(1);
    }
    else if(argc == 3){
        ascport = argv[2]; /* Read what the user gave us */
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


    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }/* So that we can re-bind to it without TIME_WAIT problems */
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR,&reuse_addr, sizeof(reuse_addr));
/* Set socket to non-blocking with our setnonblocking routine */
    setnonblocking(sock);
    /* Get the address information, and bind it to the socket */


    port = atoi(ascport); /* Use function from sockhelp toconvert to an int */
    memset((char *) &server_address, 0,sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(port);
    if (bind(sock, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
        perror("bind");
        close(sock);
        exit(EXIT_FAILURE);
    }
    /* Set up queue for incoming connections. */
    if(listen(sock,5) < 0){
        perror("listen");
        exit(EXIT_FAILURE);
    }
    printf("Server waiting for connections...\n");
    /* Since we start with only one socket, the listening socket,it is the highest socket so far. */
    highsock = sock;
    memset((char *) &connectlist, 0, sizeof(connectlist));
    while (1) { /* Main server loop - forever */
        if(catch == 1){
            deleteAll(&clients_list);   //delete list
            printf("\n SERVER EXITING...\n");
            exit(EXIT_SUCCESS);
        }
        build_select_list();

        readsocks = select(highsock + 1, &socks, (fd_set *) 0, (fd_set *) 0, NULL);
        if (readsocks < 0) {
            perror("select");
            exit(EXIT_FAILURE);
        }
        read_socks();

    }
}

void sigint_handler(int sig){
    catch=1;
}

