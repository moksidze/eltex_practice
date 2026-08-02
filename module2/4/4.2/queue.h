#ifndef _QUEUE_H
#define _QUEUE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_NAME_LEN 50

typedef struct Item {
    int priority;
    char name[MAX_NAME_LEN];
    struct Item* next;
} Item;

typedef struct Queue {
    Item* head;
    int size;
} Queue;

Queue* create_queue(void);
void enqueue(Queue* q, int priority, const char* name);
char* dequeue_first(Queue* q);
char* dequeue_by_priority(Queue* q, int priority);
char* dequeue_by_min_priority(Queue* q, int min_priority);
int is_empty(Queue* q);
int get_queue_size(Queue* q);
void print_queue(Queue* q);
void clear_queue(Queue* q);

void show_menu(void);
void add_element(Queue* q);
void extract_first(Queue* q);
void extract_by_priority(Queue* q);
void extract_by_min_priority(Queue* q);
void show_queue(Queue* q);

void run_tests(void);

#endif