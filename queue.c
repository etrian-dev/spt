#include <stdlib.h>
#include "queue.h"

void enqueue(Queue* head, Queue* tail, const int v) {
    Element* new = (Element*)malloc(sizeof(Element));
    new->value = v;
    new->next = NULL;
    if (*head == NULL) {
        *head = new;
        *tail = new;
    }
    else {
        (*tail)->next = new;
        *tail = new;
    }
}
Element* dequeue(Queue* head, Queue* tail) {
    if (*head) {
        Element* tmp = *head;
        *head = (*head)->next;
        if (*head == NULL) {
            *tail = NULL;
        }
        return tmp;
    }
    else {
        return NULL;
    }
}

int isIn(Queue head, const int key) {
    int found = 0;
    while ((found == 0) && (head != NULL)) {
        if (head->value == key) {
            found = 1;
        }
        head = head->next;
    }
    return found;
}