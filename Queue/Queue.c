#include "Queue.h"

// To Enqueue an String
void Enqueue(char *data)
{
    struct QueueNode* temp = (struct QueueNode*)malloc(sizeof(struct QueueNode));
    temp->data =data;
    temp->next = NULL;
    if(front == NULL && rear == NULL)
    {
        front = rear = temp;
        return;
    }
    rear->next = temp;
    rear = temp;
}

// To Dequeue an String
void Dequeue()
{
    struct QueueNode* temp = front;
    if(front == NULL)
    {
        printf("Queue is Empty\n");
        return;
    }
    if(front == rear)
    {
        front = rear = NULL;
    }
    else
    {
        front = front->next;
    }
    free(temp);
}

char *Front()
{
    if(front == NULL)
    {
        printf("Queue is empty\n");
        return NULL;
    }
    return front->data;
}

void Print()
{
    struct QueueNode* temp = front;
    while(temp != NULL)
    {
        printf("%s ",temp->data);
        temp = temp->next;
    }
    printf("\n");
}



