
#ifndef CLIENT_CLIENT_LIST_H
#define CLIENT_CLIENT_LIST_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <stdbool.h>

typedef struct ListNode{
    char client_address[30];
    uint16_t portNum;
    struct ListNode* next;
}ListNode;


void addNode(ListNode **, char *, uint16_t );
void deleteAll(ListNode **);
void printList(ListNode* );
bool find(ListNode* head, char *, uint16_t  );
bool delete(ListNode **,char *, uint16_t );

#endif //CLIENT_CLIENT_LIST_H
