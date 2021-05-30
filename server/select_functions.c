

#include "select_functions.h"



void deal_with_data(int listnum/* Current item in connectlist for for loops */) {
    char buffer[80];     /* Buffer for socket reads */
    int data_left_in_socket=0;
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
        printf("Received:%s;\n",buffer);
        if(strstr(buffer,"LOG_ON") != NULL){

            uint16_t portNum;
            struct sockaddr_in client_address;
            char str_address[30];
            char respond[80];

            char* temp_str = strdup(buffer);
            char* ptr = strtok(temp_str,"<");   //split and store data from buffer (format: LOG_ON<address,port>)
            ptr = strtok(NULL, ",");
            strcpy(str_address,ptr);
            ptr = strtok(NULL, ">");
            portNum = (uint16_t)atoi(ptr);

            uint16_t binary_port = htons(portNum);          //convert from int to binary port
            inet_pton(AF_INET,str_address, &client_address.sin_addr);       //store binary address



            if(find(clients_list,str_address,portNum) == false){        //if the client isn't already in list, add him
                addNode(&clients_list,str_address,portNum);
            }

            sprintf(respond,"USER_ON<%u,%u>",client_address.sin_addr.s_addr,binary_port);
            for(int i = 0; i < 5; i++) {                       //send the respond to all clients
                if(connectlist[i] != 0) {
                    if (write(connectlist[i], respond, sizeof(respond)) < 0)
                        perror("write");
                }
            }


            free(temp_str);

        }else if(strstr(buffer,"GET_CLIENTS") != NULL){
            int n = clients_in_list(clients_list);
            char respond[256];


            sprintf(respond,"CLIENT_LIST,%d",n);
            if (write(connectlist[listnum],respond, sizeof(respond)) < 0)
                perror("write");

            ListNode *current = clients_list;
            while(current!=NULL){                       //write tuple <address,port> for every client
                struct sockaddr_in client_address;
                uint16_t binary_port = htons(current->portNum);          //convert from int to binary port
                inet_pton(AF_INET,current->client_address, &client_address.sin_addr);       //store binary address


                sprintf(respond,"%u,%u",client_address.sin_addr.s_addr,binary_port);
                if (write(connectlist[listnum],respond, sizeof(respond)) < 0)
                    perror("write");
                current = current->next;
            }

        }else if(strstr(buffer,"LOG_OFF") != NULL){

            uint16_t port,binary_port;
            char ip[30];
            char address[30];
            struct sockaddr_in sa;
            char* temp_str = strdup(buffer);
            char* ptr = strtok(temp_str,"<");   //split and store data from buffer (format: LOG_OFF<address,port>)
            ptr = strtok(NULL, ",");
            strcpy(ip,ptr);
            ptr = strtok(NULL, ">");
            binary_port = atoi(ptr);
            port = ntohs((uint16_t) atoi(ptr));   //convert to host byte order

            sa.sin_addr.s_addr = (uint32_t)atoi(ip);
            inet_ntop(AF_INET, &(sa.sin_addr),address, INET_ADDRSTRLEN);  //convert to string


            if( delete_client(&clients_list,address,port) ){         //if it's in clients_list
                char respond[80];
                sprintf(respond,"USER_OFF<%s,%d>",address,binary_port);
                for(int i = 0; i < 5; i++) {
                    if(connectlist[i] != 0 && i != listnum){
                        if (write(connectlist[i], respond, strlen(respond)) < 0)            //send the respond to all clients
                            perror("write");
                    }
                }
            }else{
                char respond[80];
                strcpy(respond,"ERROR_IP_PORT_NOT_FOUND_IN_LIST" );
                if (write(connectlist[listnum],respond, sizeof(respond)) < 0)
                    perror("write");
            }

            free(temp_str);

        }
    }
}


