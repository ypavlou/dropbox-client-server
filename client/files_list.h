#ifndef CLIENT_FILES_LIST_H
#define CLIENT_FILES_LIST_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#define MAX_PATHNAME_LEN 260
typedef struct list_node{
    char path[MAX_PATHNAME_LEN];
    struct list_node* next;
}list_node;


void add(list_node **, char * );
void delete_all(list_node **);
void print(list_node* );
int files_in_list(list_node*);
#endif //CLIENT_FILES_LIST_H
