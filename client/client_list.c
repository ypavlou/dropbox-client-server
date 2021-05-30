#include "client_list.h"
void addNode(ListNode **head, char *address, uint16_t port) {
    ListNode * current  = *head;
    if(current == NULL){    //adding at the head of the list
        current = malloc(sizeof(ListNode));
        if(current == NULL){
            perror("malloc");
            exit(-1);
        }
        current->next = NULL;
        strcpy(current->client_address,address);
        current->portNum = port;
        *head = current;
        return;
    }

    //adding a node at the end of the list
    while(current->next!=NULL){
        current = current->next;
    }
    current->next = malloc(sizeof(ListNode));
    if(current->next == NULL){
        perror("malloc");
        exit(-1);
    }
    current->next->next = NULL;
    strcpy(current->next->client_address,address);
    current->next->portNum = port;

}

void deleteAll(ListNode **head){
    ListNode* current = *head;
    ListNode* next;
    while (current != NULL){
        next = current->next;
        free(current);
        current = next;
    }
    *head = NULL;
}
void printList(ListNode* head){
    ListNode* current = head;
    while(current!=NULL){
        printf("Address: %s Port: %d\n",current->client_address,current->portNum);
        current = current->next;
    }
}

bool find(ListNode* head, char *address, uint16_t port ){
    ListNode* current = head;
    while(current!=NULL){
        if(strcmp(current->client_address,address)==0 && current->portNum == port){
            return true;
        }
        current = current->next;
    }
    return false;
}
bool delete(ListNode **head,char *address, uint16_t port){

    ListNode *prev = *head;
    if(*head == NULL) //empty list
        return false;

    if(strcmp((*head)->client_address,address) == 0 && (*head)->portNum == port){
        *head = (*head)->next;
        free(prev);
        return true;
    }
    ListNode *current = (*head)->next;
    while(prev!=NULL && current!=NULL ){
        if( strcmp(current->client_address,address) == 0 && current->portNum == port){
            ListNode *temp = current;
            prev->next = current->next;
            free(temp);
            return true;
        }
        prev = current;
        current = current->next;
    }
    return false;       //not in list
}