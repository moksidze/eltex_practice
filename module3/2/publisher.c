#include "publisher.h"
#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>

static volatile sig_atomic_t running = 1;

static void sigintHandler(int sig)
{
    (void)sig;
    running = 0;
}

void publisherRun(const char *topic)
{
    int qid = getQueue(0);

    if (qid == -1)
    {
        printf("Broker is not running.\n");
        return;
    }

    signal(SIGINT, sigintHandler);

    char message[MAX_TEXT];
    char payload[256];

    snprintf(message,
             sizeof(message),
             "publisher,%d",
             getpid());

    if (sendMessage(qid, 1, message) == -1)
    {
        printf("Failed to register publisher.\n");
        return;
    }

    printf("Publisher started.\n");
    printf("Topic: %s\n", topic);
    printf("Enter messages (Ctrl+C to exit)\n");

    while (running)
    {
        printf("> ");
        fflush(stdout);

        if (fgets(payload, sizeof(payload), stdin) == NULL)
            break;

        payload[strcspn(payload, "\n")] = '\0';

        snprintf(message,
                 sizeof(message),
                 "send,%d,%s,%s",
                 getpid(),
                 topic,
                 payload);

        if (sendMessage(qid, 1, message) == -1)
        {
            if (errno == EIDRM)
                break;

            perror("msgsnd");
            break;
        }
    }

    snprintf(message,
             sizeof(message),
             "publisher_exit,%d",
             getpid());

    sendMessage(qid, 1, message);

    printf("Publisher stopped.\n");
}