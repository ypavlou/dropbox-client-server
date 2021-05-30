
#include "server_connect.h"
void connect_to_server(int sock, int port, int my_port){
    char buffer[80];
    char hostbuffer[256];
    char *IPbuffer;
    char res[256];
    char respond[80];
    char str_number[256];
    struct hostent *host_entry;
    int hostname;

    // To retrieve hostname                                  //code for finding IP address:
    hostname = gethostname(hostbuffer, sizeof(hostbuffer));
    if (hostname == -1){
        perror("gethostname");
        exit(1);
    }

    // To retrieve host information
    host_entry = gethostbyname(hostbuffer);
    if (host_entry == NULL){
        perror("gethostbyname");
        exit(1);
    }

    // To convert an Internet network
    // address into ASCII string
    IPbuffer = inet_ntoa(*((struct in_addr*)
            host_entry->h_addr_list[0]));
    if (IPbuffer == NULL){
        perror("inet_ntoa");
        exit(1);
    }

    printf("Sending LOG_ON..\n");

    sprintf(buffer,"LOG_ON<%s,%d>",IPbuffer,my_port);

    if (write(sock, buffer,sizeof(buffer)) < 0)         //write LOG_ON
        perror_exit("write");

    if (read(sock, respond, sizeof(respond)) < 0) {//read USER_ON from server
        perror_exit("read");
    }



    printf("Client <%s , %d> connected to server.\n",IPbuffer,my_port);

    bzero(buffer, sizeof(buffer));      //clear the buffer

    printf("Sending get clients..\n");
    sprintf(buffer,"GET_CLIENTS");
    if (write(sock, buffer,sizeof(buffer)) < 0)         //write GET_CLIENTS
        perror_exit("write");

    bzero(res, sizeof(res));
    if (read(sock, res, sizeof(res)) < 0) {
        perror_exit("read");
    }


    char* temp_str = strdup(res);
    char* ptr = strtok(temp_str,",");
    ptr = strtok(NULL, "\n");
    strcpy(str_number,ptr);
    for(int i = 0; i < atoi(str_number); i++){
        bzero(res, sizeof(res));
        if (read(sock, res, sizeof(res) ) < 0) {
            perror_exit("read");
        }


        struct sockaddr_in sa;
        char address[30];
        char *temp = strdup(res);
        char *p = strtok(temp,",");
        p = strtok(NULL,"\n");
        uint16_t client_port = ntohs((uint16_t) atoi(p));   //convert to host byte order

        sa.sin_addr.s_addr = (uint32_t)atoi(temp);
        inet_ntop(AF_INET, &(sa.sin_addr),address, INET_ADDRSTRLEN);  //convert to string

        add_in_list(address, client_port);
        free(temp);

    }

    printf("List of clients logged in server: \n");
    printList(clients_list);
    free(temp_str);
    get_files(IPbuffer,my_port);  //get files from all logged clients
}

void add_in_list(char *address, uint16_t port){
    if ( pthread_mutex_lock(&mtx) ){

        printf("error: mutex lock\n");
        exit(EXIT_FAILURE);
    } //lock mutex

    if(find(clients_list,address,port) == false){
        addNode(&clients_list,address,port);
    }


    if( pthread_mutex_unlock(&mtx) ){
        printf("error: mutex unlock\n");
        exit(EXIT_FAILURE);
    } //unlock mutex
}
void get_files(char *address, int my_port){
    if ( pthread_mutex_lock(&mtx) ){

        printf("error: mutex lock\n");
        exit(EXIT_FAILURE);
    } //lock mutex

    ListNode *current = clients_list;
    while(current != NULL){

        if(current->portNum != my_port) {

            connect_to_peer_2(current->client_address, current->portNum); //connect to peer and send get_file_list
        }
        current = current->next;
    }

    if( pthread_mutex_unlock(&mtx) ){
        printf("error: mutex unlock\n");
        exit(EXIT_FAILURE);
    } //unlock mutex
}

void perror_exit(char *message){
    perror(message);
    exit(EXIT_FAILURE);

}

void log_off(int socket, int my_port){
    char buffer[80];
    char hostbuffer[256];
    char *IPbuffer;
    struct hostent *host_entry;
    int hostname;

    // To retrieve hostname                                  //code for finding IP address:
    hostname = gethostname(hostbuffer, sizeof(hostbuffer));
    if (hostname == -1){
        perror("gethostname");
        exit(1);
    }

    // To retrieve host information
    host_entry = gethostbyname(hostbuffer);
    if (host_entry == NULL){
        perror("gethostbyname");
        exit(1);
    }

    // To convert an Internet network
    // address into ASCII string
    IPbuffer = inet_ntoa(*((struct in_addr*)
            host_entry->h_addr_list[0]));
    if (IPbuffer == NULL){
        perror("inet_ntoa");
        exit(1);
    }

    struct sockaddr_in client_address;
    uint16_t binary_port = htons(my_port);          //convert from int to binary port
    inet_pton(AF_INET,IPbuffer, &client_address.sin_addr);       //store binary address

    printf("LOGGING OFF from server..\n");


    sprintf(buffer,"LOG_OFF<%u,%u>",client_address.sin_addr.s_addr,binary_port);

    if (write(socket, buffer,sizeof(buffer)) < 0)         //write LOG_OFF<ip,port>
        perror("write");



}