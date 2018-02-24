#ifndef RAFT_QUEUE_H
#define RAFT_QUEUE_H

#include<stdio.h>
#include<stdlib.h>

struct QueueNode
{
    char *data;
    struct QueueNode* next;
};

extern struct QueueNode* front;
extern struct QueueNode* rear;

void Enqueue(char *data);
void Dequeue();
char *Front();
void Print();


#endif //RAFT_QUEUE_H
