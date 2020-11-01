#ifndef QUEUE_H_INCLUDED
#define QUEUE_H_INCLUDED

typedef struct element_t {
    int value;
    struct element_t* next;
} Element;
typedef Element* Queue;

void enqueue(Queue* head, Queue* tail, const int v);
Element* dequeue(Queue* head, Queue* tail);
int isIn(Queue head, const int key);

#endif
