#include <stdio.h>

typedef struct node
{
    int data;
    Node *prev;
    Node *next;
}Node;

Node* constructNode(int val)
{
    Node *head = NULL;

    head = (Node*)malloc(sizeof(Node));
    head->data = val;
    head->prev = NULL;
    head->next = NULL;

    return head;
}

void destoryNode(Node* tempNode)
{
    if (tempNode != NULL)
    {
        free(tempNode);
        tempNode = NULL;
    }
}

void pushQueueNode(Node* pHead, Node* pNode)
{
    Node *curNode = pHead;

    while (curNode->next != NULL)
    {
        curNode = curNode->next;
    }
    curNode->next = pNode;
    pNode->prev = curNode;
}

void popQueueNode(Node* pHead, Node* pNode)
{
    Node *tempNode = pHead;

    pNode->next = pHead->next;
    pHead = pNode;
    destoryNode(tempNode);
}

void destoryQueue(Node* pHead)
{
    Node *curNode = pHead;
    Node *tempNode = NULL;
    while (curNode != NULL)
    {
        tempNode = curNode;
        curNode = curNode->next;
        destoryNode(tempNode);
    }
}