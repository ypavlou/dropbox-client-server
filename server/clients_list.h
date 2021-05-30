
#ifndef SYSPRO3_CLIENTS_LIST_H
#define SYSPRO3_CLIENTS_LIST_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <netinet/in.h>
typedef struct ListNode{
    char client_address[30];
    uint16_t portNum;
    struct ListNode* next;
}ListNode;

void addNode(ListNode **, char *, uint16_t );
void deleteAll(ListNode **);
void printList(ListNode* );
bool find(ListNode*, char *, uint16_t);
int clients_in_list(ListNode*);
bool delete_client(ListNode **,char *, uint16_t );
#endif //SYSPRO3_CLIENTS_LIST_H
