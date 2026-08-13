#include "ipc_utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>

#define SHM_NAME "/producer_consumer_shm"
#define SEM_NAME "/producer_consumer_sem"

void producer_function(void)
{
    printf("Producer started (PID: %d)\n", getpid());

    srand(time(NULL) ^ (getpid() << 16));

    size_t used_memory = sizeof(SharedMemory);
    int block_count = 0;
    int blocks_since_check = 0;
    const int BATCH_SIZE = 5;

    while (1)
    {
        size_t remaining = shm->total_size - used_memory;

        if (remaining < sizeof(Node) + sizeof(int))
        {
            semaphore_lock();

            shm->producer_finished = true;

            semaphore_unlock();

            printf("Producer: no more memory, finished. Created %d blocks\n", block_count);
            break;
        }

        int count = (rand() % 50) + 20;

        if (remaining < sizeof(Node) + count * sizeof(int))
        {
            semaphore_lock();

            shm->producer_finished = true;

            semaphore_unlock();

            printf("Producer: not enough memory for next array, finished. Created %d blocks\n", block_count);
            break;
        }

        size_t node_offset = used_memory;
        size_t data_offset = node_offset + sizeof(Node);

        Node *new_node = (Node *)((char *)shm + node_offset);

        new_node->count = count;
        new_node->data_offset = data_offset;
        new_node->next_offset = 0;

        int *data = (int *)((char *)shm + data_offset);

        for (int i = 0; i < count; i++)
        {
            data[i] = (rand() % 100) + 1;
        }

        semaphore_lock();

        if (shm->head_offset == 0)
        {
            shm->head_offset = node_offset;
            shm->tail_offset = node_offset;
        }
        else
        {
            Node *tail = (Node *)((char *)shm + shm->tail_offset);

            tail->next_offset = node_offset;
            shm->tail_offset = node_offset;
        }

        used_memory += sizeof(Node) + count * sizeof(int);

        semaphore_unlock();

        block_count++;
        blocks_since_check++;

        printf("Producer: created block #%d with %d numbers\n", block_count, count);

        if (blocks_since_check >= BATCH_SIZE)
        {
            semaphore_lock();

            bool all_processed = true;

            size_t current_offset = shm->head_offset;

            while (current_offset != 0)
            {
                Node *current = (Node *)((char *)shm + current_offset);

                if (current->count > 0)
                {
                    all_processed = false;
                    break;
                }

                current_offset = current->next_offset;
            }

            semaphore_unlock();

            if (all_processed && block_count > 0)
            {
                semaphore_lock();

                shm->producer_finished = true;

                semaphore_unlock();

                printf("Producer: all blocks are processed, finishing. Created %d blocks\n", block_count);
                break;
            }

            blocks_since_check = 0;

            if (!all_processed)
            {
                printf("Producer: waiting for consumers to process blocks...\n");
                sleep(1);
            }
        }

        usleep((rand() % 100 + 50) * 1000);
    }

    printf("Producer: exiting\n");

    close_semaphore();
    detach_shared_memory();

    exit(0);
}