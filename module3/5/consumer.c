#include "ipc_utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>

#define SHM_NAME "/producer_consumer_shm"
#define SEM_NAME "/producer_consumer_sem"

void consumer_function(int id)
{
    printf("Consumer %d started (PID: %d)\n", id, getpid());

    srand(time(NULL) ^ (getpid() << 16) ^ (id << 8));

    int processed_blocks = 0;
    int wait_count = 0;

    while (1)
    {
        bool producer_done = false;
        bool has_unprocessed = false;

        semaphore_lock();

        producer_done = shm->producer_finished;

        size_t current_offset = shm->head_offset;

        while (current_offset != 0)
        {
            Node *current = (Node *)((char *)shm + current_offset);

            if (current->count > 0)
            {
                has_unprocessed = true;
                break;
            }

            current_offset = current->next_offset;
        }

        semaphore_unlock();

        if (producer_done && !has_unprocessed)
        {
            printf("Consumer %d: all blocks processed, exiting\n", id);
            break;
        }

        if (!has_unprocessed && !producer_done)
        {
            wait_count++;

            if (wait_count % 5 == 0)
            {
                printf("Consumer %d: waiting for new blocks... (%d waits)\n", id, wait_count);
            }

            usleep((rand() % 300 + 100) * 1000);

            continue;
        }

        wait_count = 0;

        semaphore_lock();

        current_offset = shm->head_offset;

        while (current_offset != 0)
        {
            Node *current = (Node *)((char *)shm + current_offset);

            if (current->count > 0)
            {
                break;
            }

            current_offset = current->next_offset;
        }

        if (current_offset == 0)
        {
            semaphore_unlock();

            usleep((rand() % 300 + 100) * 1000);

            continue;
        }

        Node *current = (Node *)((char *)shm + current_offset);

        int count = current->count;

        int *data = (int *)((char *)shm + current->data_offset);

        current->count = 0;

        semaphore_unlock();

        int min_val = data[0];
        int max_val = data[0];

        for (int i = 1; i < count; i++)
        {
            if (data[i] < min_val)
            {
                min_val = data[i];
            }

            if (data[i] > max_val)
            {
                max_val = data[i];
            }
        }

        processed_blocks++;

        printf("Consumer %d: processed block with %d numbers, min=%d, max=%d\n", id, count, min_val, max_val);
        usleep((rand() % 300 + 100) * 1000);
    }

    printf("Consumer %d: processed total %d blocks\n", id, processed_blocks);

    close_semaphore();
    detach_shared_memory();

    exit(0);
}