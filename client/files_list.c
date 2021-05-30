
#include "files_list.h"
void add(list_node **head, char *p) {
    list_node * current  = *head;
    if(current == NULL){    //adding at the head of the list
        current = malloc(sizeof(list_node));
        if(current == NULL){
            perror("malloc");
            exit(-1);
        }
        current->next = NULL;
        strcpy(current->path,p);
        *head = current;
        return;
    }

    //adding a node at the end of the list
    while(current->next!=NULL){
        current = current->next;
    }
    current->next = malloc(sizeof(list_node));
    if(current->next == NULL){
        perror("malloc");
        exit(-1);
    }
    current->next->next = NULL;
    strcpy(current->next->path,p);

}

void delete_all(list_node **head){
    list_node* current = *head;
    list_node* next;
    while (current != NULL){
        next = current->next;
        free(current);
        current = next;
    }
    *head = NULL;
}
void print(list_node* head){
    list_node* current = head;
    while(current!=NULL){
        printf("Path is %s\n",current->path);
        current = current->next;
    }
}

int files_in_list(list_node* head){
    list_node* current = head;
    int count = 0;
    while(current!=NULL){
        count++;
        current = current->next;
    }
    return count;
}