#ifndef IPC_UTILS_H
#define IPC_UTILS_H

#include <stdbool.h>
#include <stddef.h>
#include <semaphore.h>

typedef struct Node {
    int count;
    size_t data_offset;
    size_t next_offset;
} Node;

typedef struct {
    size_t head_offset;
    size_t tail_offset;
    size_t memory_start;
    size_t memory_end;
    size_t total_size;
    bool producer_finished;
} SharedMemory;

extern int shm_fd;
extern SharedMemory *shm;
extern sem_t *semaphore;

int init_shared_memory(const char *name, size_t size);
int attach_shared_memory(const char *name, size_t size);
void detach_shared_memory(void);
void cleanup_shared_memory(const char *name);

int init_semaphore(const char *name);
int open_semaphore(const char *name);
void close_semaphore(void);
void cleanup_semaphore(const char *name);

void semaphore_lock(void);
void semaphore_unlock(void);

#endif