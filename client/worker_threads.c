#include "worker_threads.h"
#include "server_connect.h"
#include "files_list.h"

void * work(void * ptr){
    while (1) {
        if(pool.count > 0) {
            int num_of_data = check_data(&pool);
            printf("Num of data : %d\n", num_of_data);
            if (num_of_data == 2) {                       //ip, port
                get_tuple_2(&pool);

            } else if (num_of_data == 4) {                 //ip, port, path, version
                get_tuple_4(&pool);
            }

            pthread_cond_signal(&pool_cond_nonfull);        //a place in pool is released wake up another thread

        }
    }
    pthread_exit(0);
}

void get_tuple_2(pool_t * p) {
    pthread_mutex_lock(&pool_mtx);
    while (p->count <= 0) {
        printf(">> Found Buffer Empty \n");
        pthread_cond_wait(&pool_cond_nonempty, &pool_mtx);
    }
    char address[30];
    uint16_t port = p->portNum[p->start];
    strcpy(address, p->ipAddr[p->start]);

    p->portNum[p->start] = 0;               //clear these data
    strcpy(p->ipAddr[p->start],"empty");

    p->start = (p->start + 1) % POOL_SIZE;  //remove from buffer
    p->count--;
    pthread_mutex_unlock(&pool_mtx);
    printf("\n Thread %ld just removed item from pool <%s, %d>\n", pthread_self(),address,port);

    connect_to_peer_2(address, port);       //connect to peer client

}

void connect_to_peer_2(char *address, uint16_t port){
    int    peer_sock;
    struct sockaddr_in peer_client;
    struct sockaddr *peerptr = (struct sockaddr*)&peer_client;
    struct hostent *rem;


    if ((peer_sock = socket(PF_INET, SOCK_STREAM, 0)) < 0)   //create socket fd for connection to client
        perror_exit("socket");


    if ((rem = gethostbyname(address)) == NULL) {      //find client address
        herror("gethostbyname");
        exit(1);
    }
    peer_client.sin_family = AF_INET;        //internet domain
    memcpy(&peer_client.sin_addr, rem->h_addr, rem->h_length);
    peer_client.sin_port = htons(port);         //server port

   //connection
    while(1){
        if(connect(peer_sock, peerptr, sizeof(peer_client)) >= 0)
            break;
    }

    printf("\nConnecting to peer client: %s port %d\n", address, port);

    send_get_file_list(peer_sock, address, port);

    printf("Exiting from peer client\n");
    close(peer_sock);
}
void send_get_file_list(int sock, char *address, uint16_t port){
    char buffer[80];
    char res[30];

    printf("Requesting files from client\n");
    strcpy(buffer,"GET_FILE_LIST");
    if (write(sock, buffer,sizeof(buffer)) < 0)         //write GET_FILE_LIST
        perror_exit("write");

    bzero(res, sizeof(res));
    if (read(sock, res, sizeof(res)) < 0) {     //read FILE_LIST,n
        perror_exit("read");
    }


    char* temp_str = strdup(res);
    char* ptr = strtok(temp_str,",");
    ptr = strtok(NULL, "\n");

    for(int i = 0; i < atoi(ptr); i++){         //for n

        char buf[MAX_PATHNAME_LEN];
        bzero(buf, sizeof(buf));
        if (read(sock, buf, sizeof(buf) ) < 0) {        //read pathname
            perror_exit("read");
        }


        time_t version;
        if (read(sock, &version, sizeof(time_t) ) < 0) {        //read last modification time
            perror_exit("read");
        }

        add_4(&pool, address, port, buf, version);          //add the tuple to the buffer
        pthread_cond_signal(&pool_cond_nonfull);        //pool is not empty anymore wake up another thread


    }
    free(temp_str);
}

bool check_if_exists(char *address, uint16_t port){
    if ( pthread_mutex_lock(&mtx) ){

        printf("error: mutex lock\n");
        exit(EXIT_FAILURE);
    } //lock mutex

    bool r = find(clients_list,address,port);

    if( pthread_mutex_unlock(&mtx) ){
        printf("error: mutex unlock\n");
        exit(EXIT_FAILURE);
    } //unlock mutex

    return r;
}
void get_tuple_4(pool_t * p) {
    pthread_mutex_lock(&pool_mtx);
    while (p->count <= 0) {
        printf(">> Found Buffer Empty \n");
        pthread_cond_wait(&pool_cond_nonempty, &pool_mtx);
    }
    char address[30];
    char pathname[MAX_PATHNAME_LEN];
    time_t version = p->version[p->start];
    uint16_t port = p->portNum[p->start];
    strcpy(address, p->ipAddr[p->start]);
    strcpy(pathname,p->pathname[p->start]);

    p->portNum[p->start] = 0;               //clear these data
    strcpy(p->ipAddr[p->start],"empty");
    strcpy(p->pathname[p->start],"empty");
    p->version[p->start] = 0;

    p->start = (p->start + 1) % POOL_SIZE;  //remove from buffer
    p->count--;
    pthread_mutex_unlock(&pool_mtx);

    printf("\n Thread %ld just removed item from pool <%s, %d, %s, %ld>\n", pthread_self(),address,port, pathname, version);


    if(check_if_exists(address, port) == true){     //client is in list
        char path_of_directory[MAX_PATHNAME_LEN];
        sprintf(path_of_directory,"%s/client_%s_%u",dirName,address,port);       //directory ...dirName/client_address_port


        if( mkdir(path_of_directory,0777) && errno != EEXIST ) //create ...dirName/client_address_port if it doesn't already exist
            printf("error while trying to create %s",path_of_directory );

        char file[MAX_PATHNAME_LEN];
        sprintf(file,"%s/%s",path_of_directory,basename(pathname));     //take the name of file with basename()

        if( access( file, F_OK ) != -1 ) {// file exists
            printf("\nFile %s exists in directory, checking version ...\n",file);

            struct stat fst;
            if (stat(file, &fst) != 0) {
                perror("stat");
            }
            time_t my_version = fst.st_mtime;

            connect_to_peer_4(address, port, pathname, my_version,file);


        } else {
            // file doesn't exist
            printf("\nFile %s not in directory, sending a file request ...\n",file);
            connect_to_peer_4(address, port, pathname, 0,file);
        }

    }else{          //client not in list
        printf("Client not in list\n");
    }


}
void read_file(int sock, int bytes, char *filename){
    char* buffer = malloc((size_t)bytes+1);
    if(buffer == NULL){
        perror("malloc");
    }

    int in=read(sock, buffer, (size_t)bytes);

    if(in != bytes){
        perror("read bytes");
    }

    buffer[in] = '\0';

    FILE* fp = fopen(filename, "a");
    if(fp==NULL){
        perror("file");
    }
    fputs(buffer,fp); //copy the content of file in the file in mirror dir
    fclose(fp);

    free(buffer);

}
void connect_to_peer_4(char *address, uint16_t port, char *pathname, time_t version, char* file){
    int    peer_sock;
    struct sockaddr_in peer_client;
    struct sockaddr *peerptr = (struct sockaddr*)&peer_client;
    struct hostent *rem;


    if ((peer_sock = socket(PF_INET, SOCK_STREAM, 0)) < 0)   //create socket fd for connection to client
        perror_exit("socket");

    printf("the address is %s\n",address);

    if ((rem = gethostbyname(address)) == NULL) {      //find client address
        herror("gethostbyname");
        exit(1);
    }
    peer_client.sin_family = AF_INET;        //internet domain
    memcpy(&peer_client.sin_addr, rem->h_addr, rem->h_length);
    peer_client.sin_port = htons(port);         //peer port

    while(1){
        if(connect(peer_sock, peerptr, sizeof(peer_client)) >= 0)
            break;
    }

    //connection


    printf("\nConnecting to peer client: %s port %d\n", address, port);

    send_get_file(peer_sock, address, port,pathname,version, file);

    printf("Exiting from peer client\n");
    close(peer_sock);
}

void send_get_file(int sock, char *address, uint16_t port, char *path, time_t ver, char *file){
    char buffer[80];
    char res[80];

    time_t version = ver;

    printf("Requesting file from client.\n");

    sprintf(buffer,"GET_FILE,%s,%ld",path,version);

    if (write(sock, buffer,sizeof(buffer)) < 0)         //write GET_FILE,path,version
        perror_exit("write");



    bzero(res, sizeof(res));
    if (read(sock, res, sizeof(res)) < 0) {     //read reply
        perror_exit("read");
    }
    if(strstr(res, "FILE_NOT_FOUND") != NULL){      //peer didn't find the file
        printf("\nFile not found by peer\n");

    }else if(strstr(res, "FILE_UP_TO_DATE") != NULL){
        printf("\nFile is up to date\n");

    }else if(strstr(res, "FILE_SIZE") != NULL){ //the peer is sending the file

        int bytes;
        if (read(sock, &bytes, sizeof(bytes)) < 0) {     //read bytes of file
            perror_exit("read");
        }

        time_t date;

        if (read(sock, &date, sizeof(date)) < 0) {     //read date-version of file
            perror_exit("read");
        }


        read_file(sock,bytes,file);
        printf("File: %s copied.\n",file);
        char command[256];
        snprintf(command, sizeof command, "touch -d '%s' %s",ctime(&date) ,file);       //set new modification time
        system(command);

    }

}