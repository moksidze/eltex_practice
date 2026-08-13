#include "ipc_utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

int shm_fd = -1;
SharedMemory *shm = NULL;
sem_t *semaphore = NULL;

int init_shared_memory(const char *name, size_t size)
{
    shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);

    if (shm_fd == -1)
    {
        perror("shm_open");
        return -1;
    }

    if (ftruncate(shm_fd, size) == -1)
    {
        perror("ftruncate");
        close(shm_fd);
        shm_fd = -1;
        return -1;
    }

    shm = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    if (shm == MAP_FAILED)
    {
        perror("mmap");
        close(shm_fd);
        shm_fd = -1;
        shm = NULL;
        return -1;
    }

    memset(shm, 0, size);

    shm->memory_start = sizeof(SharedMemory);
    shm->memory_end = size;
    shm->total_size = size;
    shm->head_offset = 0;
    shm->tail_offset = 0;
    shm->producer_finished = false;

    return 0;
}

int attach_shared_memory(const char *name, size_t size)
{
    shm_fd = shm_open(name, O_RDWR, 0666);

    if (shm_fd == -1)
    {
        perror("shm_open");
        return -1;
    }

    shm = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    if (shm == MAP_FAILED)
    {
        perror("mmap");
        close(shm_fd);
        shm_fd = -1;
        shm = NULL;
        return -1;
    }

    return 0;
}

void detach_shared_memory(void)
{
    if (shm != NULL)
    {
        munmap(shm, shm->total_size);
        shm = NULL;
    }

    if (shm_fd != -1)
    {
        close(shm_fd);
        shm_fd = -1;
    }
}

void cleanup_shared_memory(const char *name)
{
    detach_shared_memory();
    shm_unlink(name);
}

int init_semaphore(const char *name)
{
    semaphore = sem_open(name, O_CREAT | O_EXCL, 0666, 1);

    if (semaphore == SEM_FAILED)
    {
        if (errno == EEXIST)
        {
            semaphore = sem_open(name, 0);
        }

        if (semaphore == SEM_FAILED)
        {
            perror("sem_open");
            semaphore = NULL;
            return -1;
        }
    }

    return 0;
}

int open_semaphore(const char *name)
{
    semaphore = sem_open(name, 0);

    if (semaphore == SEM_FAILED)
    {
        perror("sem_open");
        semaphore = NULL;
        return -1;
    }

    return 0;
}

void close_semaphore(void)
{
    if (semaphore != NULL)
    {
        sem_close(semaphore);
        semaphore = NULL;
    }
}

void cleanup_semaphore(const char *name)
{
    close_semaphore();
    sem_unlink(name);
}

void semaphore_lock(void)
{
    if (sem_wait(semaphore) == -1)
    {
        perror("sem_wait");
        exit(EXIT_FAILURE);
    }
}

void semaphore_unlock(void)
{
    if (sem_post(semaphore) == -1)
    {
        perror("sem_post");
        exit(EXIT_FAILURE);
    }
}