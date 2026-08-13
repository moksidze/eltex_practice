#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include "ipc_utils.h"

#define SHM_NAME "/producer_consumer_shm"
#define SEM_NAME "/producer_consumer_sem"
#define SHM_SIZE (2 * 1024)

void producer_function(void);
void consumer_function(int id);

void signal_handler(int sig)
{
    printf("\nReceived signal %d, cleaning up...\n", sig);

    cleanup_shared_memory(SHM_NAME);
    cleanup_semaphore(SEM_NAME);

    exit(0);
}

int main(int argc, char *argv[])
{
    int num_consumers = 1;

    if (argc >= 2)
    {
        num_consumers = atoi(argv[1]);

        if (num_consumers < 1)
        {
            printf("Number of consumers must be >= 1\n");
            return 1;
        }
    }

    printf("   Producer-Consumer Model   \n");
    printf("Number of consumers: %d\n", num_consumers);

    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    if (init_shared_memory(SHM_NAME, SHM_SIZE) == -1)
    {
        return 1;
    }

    if (init_semaphore(SEM_NAME) == -1)
    {
        cleanup_shared_memory(SHM_NAME);
        return 1;
    }

    pid_t producer_pid = fork();

    if (producer_pid == -1)
    {
        perror("fork producer");
        cleanup_shared_memory(SHM_NAME);
        cleanup_semaphore(SEM_NAME);
        return 1;
    }

    if (producer_pid == 0)
    {
        producer_function();
        return 0;
    }

    pid_t *consumer_pids = malloc(num_consumers * sizeof(pid_t));

    if (consumer_pids == NULL)
    {
        perror("malloc");
        cleanup_shared_memory(SHM_NAME);
        cleanup_semaphore(SEM_NAME);
        return 1;
    }

    for (int i = 0; i < num_consumers; i++)
    {
        consumer_pids[i] = fork();

        if (consumer_pids[i] == -1)
        {
            perror("fork consumer");
            free(consumer_pids);
            cleanup_shared_memory(SHM_NAME);
            cleanup_semaphore(SEM_NAME);
            return 1;
        }

        if (consumer_pids[i] == 0)
        {
            consumer_function(i + 1);
            return 0;
        }
    }

    int status;

    for (int i = 0; i < num_consumers; i++)
    {
        waitpid(consumer_pids[i], &status, 0);
    }

    waitpid(producer_pid, &status, 0);

    printf("\nAll processes finished\n");

    free(consumer_pids);

    cleanup_shared_memory(SHM_NAME);
    cleanup_semaphore(SEM_NAME);

    return 0;
}