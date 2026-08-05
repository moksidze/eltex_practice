#include "subscriber.h"
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

void subscriberRun(int topicCount, char *topics[])
{
    int qid = getQueue(0);

    if (qid == -1)
    {
        printf("Broker is not running.\n");
        return;
    }

    signal(SIGINT, sigintHandler);

    char message[MAX_TEXT];

    for (int i = 0; i < topicCount; i++)
    {
        snprintf(message,
                 sizeof(message),
                 "subscribe,%d,%s",
                 getpid(),
                 topics[i]);

        if (sendMessage(qid, 1, message) == -1)
        {
            printf("Failed to subscribe to %s\n", topics[i]);
            return;
        }
    }

    printf("Subscriber started (PID = %d)\n", getpid());

    while (running)
    {
        Message msg;

        if (receiveMessage(qid, getpid(), &msg) == -1)
        {
            if (errno == EIDRM)
                break;

            if (!running)
                break;

            continue;
        }

        printf("\n========== MESSAGE ==========\n");
        printf("%s\n", msg.text);
        printf("=============================\n");
    }

    for (int i = 0; i < topicCount; i++)
    {
        snprintf(message,
                 sizeof(message),
                 "unsubscribe,%d,%s",
                 getpid(),
                 topics[i]);

        sendMessage(qid, 1, message);
    }

    printf("Subscriber stopped.\n");
}