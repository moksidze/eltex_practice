#ifndef IPC_UTILS_H
#define IPC_UTILS_H

#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

typedef struct Node {
    int count; // количество чисел             
    int* data; // указатель на массив чисел          
    struct Node* next;      
} Node;

typedef struct {
    Node* head;             
    Node* tail;             
    void* memory_start;     
    void* memory_end;       
    size_t total_size;      
    bool producer_finished; 
} SharedMemory; //структура содержит информацию о разделяемой памяти


union semun {
    int val; //храним значение семафора
    struct semid_ds *buf; //храним указатель на структуру с информацией о семафоре
    unsigned short *array; //храним указатель на массив значений семафоров
    struct seminfo *__buf; //храним указатель на информацию о системе семафоров.
};


extern int shmid;           
extern int semid;           
extern SharedMemory* shm;   

int init_shared_memory(key_t key, size_t size);

void detach_shared_memory();

void cleanup_shared_memory();

int init_semaphore(key_t key);

void semaphore_lock(int semid);

void semaphore_unlock(int semid);

void cleanup_semaphore();

void cleanup_all_ipc();

#endif
