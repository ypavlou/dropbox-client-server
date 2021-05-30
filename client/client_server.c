
#include "client_server.h"

void setnonblocking(soock)int soock;{       //code for select from http://cgi.di.uoa.gr/%7Emema/courses/k24/lectures/topic5-Sockets.pdf
    int opts;
    opts = fcntl(soock, F_GETFL);
    if (opts < 0) {
        perror("fcntl(F_GETFL)");
        exit(EXIT_FAILURE);
    }
    opts = (opts | O_NONBLOCK);
    if (fcntl(soock, F_SETFL, opts) < 0) {
        perror("fcntl(F_SETFL)");/* Highest #'d file desc, needed for select() */
        exit(EXIT_FAILURE);
    }
}

void build_select_list() {
    int listnum;/* CurrenaddNodet item in connectlist for for loops */
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

    printf("connection established with IP : %s and PORT : %d\n", ip, ntohs(client_connected.sin_port));
    setnonblocking(connection);
    for (listnum = 0; (listnum < 5) && (connection != -1); listnum++) {

        if (connectlist[listnum] == 0) {
            printf("\nConnection accepted:   FD=%d; Slot=%d\n", connection, listnum);
            connectlist[listnum] = connection;
            connection = -1;
        }
    }
    if (connection != -1) {/* No room left in the queue! */
        printf("\nNo room left for new client.\n");
        //sock_puts(connection, "Sorry, this server is too busy.Try again later!\r\n");
        close(connection);
    }
}

void read_socks(char *dir) {
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
            deal_with_data(listnum,dir);
        }
    } /* for (all entries in queue) */
}

void findAllfiles(char *name,list_node** head){         //idea from : https://stackoverflow.com/questions/8436841/how-to-recursively-list-directories-in-c-on-linux
    DIR *dir;
    struct dirent *entry;
    char path[MAX_PATHNAME_LEN];
    char fullpath[MAX_PATHNAME_LEN];
    strcpy(fullpath,name);

    if (!(dir = opendir(name)))//if the directory does not exist
        return;

    while ((entry = readdir(dir)) != NULL) {//for every directory in this path
        if (entry->d_type == DT_DIR) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0 || strstr(entry->d_name,"client_")!=NULL)//if the directory is current or parent's
                continue;
            sprintf(path, "%s/%s", name, entry->d_name);
            sprintf(fullpath,"%s/%s",name,entry->d_name);
            findAllfiles(path,head);
        } else {
            sprintf(fullpath,"%s/%s",name,entry->d_name);
            add(head,fullpath);     //add in file list
        }
    }
    closedir(dir);
}

bool file_exists(char *name) {
    if (access(name, F_OK) != -1) {
        // file exists
        return true;
    } else {
        // file doesn't exist
        return false;
    }
}
bool file_changed(char *file, time_t date){

    struct stat fst;

    if (stat(file, &fst) != 0) {
        perror("stat");
    }
    time_t curr_date = fst.st_mtime;
    double seconds = difftime(curr_date, date);     //compare the dates
    if (seconds > 0) {                  // curr_date > date
        return true;
    }
    return false;
}

void write_file(int fd,char *file,int bytes ){
    char* buffer;
    buffer = malloc(bytes+1);
    if(buffer == NULL){
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    FILE* in;

    size_t n = 0;
    int c;
    in = fopen(file, "r");
    if(in == NULL){
        printf("Cannot find file at path %s\n",file);
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    while ((char)(c = fgetc(in)) != EOF && n != bytes ){   //read the file char by char
        buffer[n] = (char) c;           //store it
        n++;
    }


    buffer[n] = '\0';
    fclose(in);

    int w = write(fd, buffer, n);       //write in socket all file
    if(w!=n){
        perror("write");
        exit(EXIT_FAILURE);
    }
    free(buffer);

}
void deal_with_data(int listnum,char *dir){
    char buffer[80];     /* Buffer for socket reads */
    int data_left_in_socket=0;

    if(catch ==1){
        return;
    }


    ioctl(connectlist[listnum], FIONREAD,&data_left_in_socket);
    if(data_left_in_socket<=0) {        //If there is nothing left to read in socket
        return;
    }
    bzero(buffer, sizeof(buffer));
    if (read(connectlist[listnum], buffer,(size_t) data_left_in_socket) < 0) {/* Connection closed, close this end and free up entry in connectlist */
        printf("\nConnection lost: FD=%d;  Slot=%d\n", connectlist[listnum], listnum);
        close(connectlist[listnum]);
        connectlist[listnum] = 0;
    }
    else {
        printf("Received:%s;\n", buffer);

        list_node *list = NULL;
        findAllfiles(dir,&list);

        if (strstr(buffer, "GET_FILE_LIST") != NULL) {

            struct stat fst;
            list_node *curr = list;

            int count = files_in_list(list);
            char respond[30];
            sprintf(respond,"FILE_LIST,%d",count);

            if (write(connectlist[listnum],respond, sizeof(respond)) < 0)       //write FILE_LIST,n
                perror("write");

            while(curr!=NULL) {
                char buf[MAX_PATHNAME_LEN];
                strcpy(buf,curr->path);

                if (write(connectlist[listnum],buf, sizeof(buf)) < 0)       //write path name of file
                    perror("write");

                bzero(&fst, sizeof(fst));

                if (stat(curr->path, &fst) != 0) {
                    perror("stat");
                }
                if (write(connectlist[listnum],&fst.st_mtime, sizeof(time_t)) < 0)       //version is the last modified time of file
                    perror("write");
                curr = curr->next;
            }


        }else if(strstr(buffer, "GET_FILE") != NULL){
            char file_path[MAX_PATHNAME_LEN];
            char* temp_str = strdup(buffer);
            char* ptr = strtok(temp_str,",");   //split and store data from buffer (format: GET_FILE,path,version)
            ptr = strtok(NULL, ",");
            strcpy(file_path,ptr);
            ptr = strtok(NULL, "\0");
            time_t date = atol(ptr);

            free(temp_str);


            if( file_exists(file_path) == false ){          //check if file exists
                char respond[80];
                strcpy(respond,"FILE_NOT_FOUND");
                if (write(connectlist[listnum],respond, sizeof(respond)) < 0)       //write message
                    perror("write");
            }else{
                if(file_changed(file_path, date) == false && date!=0){         //no changes in file
                    char respond[80];
                    strcpy(respond,"FILE_UP_TO_DATE");
                    if (write(connectlist[listnum],respond, sizeof(respond)) < 0)       //write message
                        perror("write");
                }else{
                    FILE *fp = fopen(file_path, "r");

                    if (fp == NULL) {
                        fclose(fp);
                        perror("open");
                    }
                    fseek(fp, 0L, SEEK_END);
                    // calculating the size of the file
                    int bytes = ftell(fp);
                    fclose(fp);

                    char respond[80];
                    strcpy(respond,"FILE_SIZE");
                    if (write(connectlist[listnum],respond, sizeof(respond)) < 0)       //write FILE_SIZE
                        perror("write");

                    if (write(connectlist[listnum],&bytes, sizeof(bytes)) < 0)       //write n
                        perror("write");


                    struct stat fst;
                    if (stat(file_path, &fst) != 0) {
                        perror("stat");
                    }
                    time_t curr_date = fst.st_mtime;
                    if (write(connectlist[listnum],&curr_date, sizeof(time_t)) < 0)       //write new modification time as version
                        perror("write");
                    write_file(connectlist[listnum], file_path, bytes);                     // write all the file

                }
            }


        }else if(strstr(buffer, "USER_OFF") != NULL){
            uint16_t port;
            struct sockaddr_in sa;
            char ip[30];
            char address[30];
            char* temp_str = strdup(buffer);
            char* ptr = strtok(temp_str,"<");   //split and store data from buffer (format: USER_OFF<address,port>)
            ptr = strtok(NULL, ",");
            strcpy(ip,ptr);
            ptr = strtok(NULL, ">");
            port = ntohs((uint16_t)atoi(ptr)); //convert to host byte order

            sa.sin_addr.s_addr = (uint32_t)atoi(ip);
            inet_ntop(AF_INET, &(sa.sin_addr),address, INET_ADDRSTRLEN);  //convert to string



            printf("\nUser with ip:%s and port: %d logged off\n",address,port);


            if ( pthread_mutex_lock(&mtx) ){

                printf("error: mutex lock\n");
                exit(EXIT_FAILURE);
            } //lock mutex

            if(delete(&clients_list, address, port) == true){
                printf("Client deleted from list.\n");
            }


            if( pthread_mutex_unlock(&mtx) ){
                printf("error: mutex unlock\n");
                exit(EXIT_FAILURE);
            } //unlock mutex




        }else if(strstr(buffer, "USER_ON") != NULL){
            char str_address[30];
            char address[30];
            struct sockaddr_in sa;
            uint16_t port;
            char* temp_str = strdup(buffer);
            char* ptr = strtok(temp_str,"<");   //split and store data from buffer (format: USER_ON<address,port>)
            ptr = strtok(NULL, ",");
            strcpy(str_address,ptr);
            ptr = strtok(NULL, ">");
            port = ntohs((uint16_t)atoi(ptr)); //convert to host byte order

            sa.sin_addr.s_addr = (uint32_t)atoi(str_address);
            inet_ntop(AF_INET, &(sa.sin_addr),address, INET_ADDRSTRLEN);  //convert to string

            addNode(&clients_list, address, port);          //add client in client list

            add_2(&pool,address,port );             //add to the buffer the ip address and the port number of the client that logged in





        }
        delete_all(&list);      //delete files list
    }
}
void create_client_server(int port, char *dir, int client_sock){

    struct sockaddr_in server_address; /* bind info structure */
    int reuse_addr = 1;  /* Used so we can re-bind to our portwhile a previous connection is stillin TIME_WAIT state. */
    int readsocks;  /* Number of sockets ready for reading */



    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR,&reuse_addr, sizeof(reuse_addr));
/* Set socket to non-blocking with our setnonblocking routine */
    setnonblocking(sock);
    /* Get the address information, and bind it to the socket */

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
    printf("\nWaiting for connections...\n");
    /* Since we start with only one socket, the listening socket,it is the highest socket so far. */
    highsock = sock;
    memset((char *) &connectlist, 0, sizeof(connectlist));

    //put client's socket with server in connectionList for receiving USER_ON and USER_OFF

     connectlist[0] = client_sock;


    int flag = 0;
    pthread_t *threads = NULL;
    while (1) { /* Main server loop - forever */
        build_select_list();

        readsocks = select(highsock + 1, &socks, (fd_set *) 0, (fd_set *) 0, NULL);
        if(errno == EINTR && catch ==1){
            free(threads);
            break;
        }
        if (readsocks < 0) {
            perror("select");
            exit(EXIT_FAILURE);
        }

        read_socks(dir);

        if(flag == 0){

            threads=malloc(workerThreads* sizeof(pthread_t));

            for(int i=0; i<workerThreads; i++){ //create threads

                if (pthread_create(threads+i, NULL, work,0) !=0) {
                    printf("Error trying to create threads\n");
                    exit(EXIT_FAILURE);
                }
            }

            flag = 1;
        }
        if( catch ==1){
            free(threads);
            break;
        }


    }
}
void deleteALL(){
    if ( pthread_mutex_lock(&mtx) ){

        printf("error: mutex lock\n");
        exit(EXIT_FAILURE);
    } //lock mutex

    deleteAll(&clients_list); //delete client list

    if( pthread_mutex_unlock(&mtx) ){
        printf("error: mutex unlock\n");
        exit(EXIT_FAILURE);
    } //unlock mutex

    if ( pthread_mutex_lock(&pool_mtx) ){

        printf("error: mutex lock\n");
        exit(EXIT_FAILURE);
    } //lock mutex

    delete_pool(&pool); //delete pool

    if( pthread_mutex_unlock(&pool_mtx) ){
        exit(EXIT_FAILURE);
    } //unlock mutex

    if (pthread_mutex_destroy(&mtx) != 0) {
        exit(EXIT_FAILURE);
    }
    if (pthread_mutex_destroy(&pool_mtx)!= 0) {
        exit(EXIT_FAILURE);
    }

    if (pthread_cond_destroy(&pool_cond_nonempty) != 0) {
        exit(EXIT_FAILURE);
    }
    if (pthread_cond_destroy(&pool_cond_nonfull) != 0) {
        exit(EXIT_FAILURE);
    }




}
