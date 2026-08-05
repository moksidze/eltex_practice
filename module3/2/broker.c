#include "broker.h"
#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/msg.h>
#include <time.h>
#include <errno.h>

#define SHUTDOWN_TIMEOUT 5

static Subscriber *subscribers = NULL;
static Publisher *publishers = NULL;

static int qid = -1;
static volatile sig_atomic_t running = 1;

static void sigintHandler(int sig)
{
    (void)sig;
    running = 0;
}

static void addPublisher(pid_t pid)
{
    Publisher *current = publishers;

    while (current != NULL)
    {
        if (current->pid == pid)
            return;

        current = current->next;
    }

    Publisher *node = malloc(sizeof(Publisher));

    if (node == NULL)
        return;

    node->pid = pid;
    node->next = publishers;
    publishers = node;
}

static void removePublisher(pid_t pid)
{
    Publisher *prev = NULL;
    Publisher *current = publishers;

    while (current != NULL)
    {
        if (current->pid == pid)
        {
            if (prev == NULL)
                publishers = current->next;
            else
                prev->next = current->next;

            free(current);
            return;
        }

        prev = current;
        current = current->next;
    }
}

static void addSubscriber(pid_t pid, const char *topic)
{
    Subscriber *current = subscribers;

    while (current != NULL)
    {
        if (current->pid == pid &&
            strcmp(current->topic, topic) == 0)
            return;

        current = current->next;
    }

    Subscriber *node = malloc(sizeof(Subscriber));

    if (node == NULL)
        return;

    node->pid = pid;

    strncpy(node->topic,
            topic,
            MAX_TOPIC - 1);

    node->topic[MAX_TOPIC - 1] = '\0';

    node->next = subscribers;
    subscribers = node;
}

static void removeSubscriber(pid_t pid, const char *topic)
{
    Subscriber *prev = NULL;
    Subscriber *current = subscribers;

    while (current != NULL)
    {
        if (current->pid == pid &&
            strcmp(current->topic, topic) == 0)
        {
            if (prev == NULL)
                subscribers = current->next;
            else
                prev->next = current->next;

            free(current);
            return;
        }

        prev = current;
        current = current->next;
    }
}

static void forwardMessage(pid_t sender,
                           const char *topic,
                           const char *payload)
{
    char buffer[MAX_TEXT];

    snprintf(buffer,
             sizeof(buffer),
             "publisher=%d topic=%s\n%s",
             sender,
             topic,
             payload);

    Subscriber *current = subscribers;

    while (current != NULL)
    {
        Subscriber *next = current->next;

        if (strcmp(current->topic, topic) == 0)
        {
            if (sendMessage(qid,
                            current->pid,
                            buffer) == -1)
            {
                removeSubscriber(current->pid,
                                 current->topic);
            }
        }

        current = next;
    }
}

void brokerRun(void)
{
    qid = getQueue(1);

    if (qid == -1)
    {
        printf("Broker is already running.\n");
        return;
    }

    signal(SIGINT, sigintHandler);

    printf("Broker started.\n");

    while (running)
    {
        Message msg;

        if (receiveMessage(qid, 1, &msg) == -1)
        {
            if (!running)
                break;

            continue;
        }

        char *command = strtok(msg.text, ",");
        char *pidStr = strtok(NULL, ",");
        char *topic = strtok(NULL, ",");

        if (command == NULL || pidStr == NULL)
            continue;

        pid_t pid = (pid_t)atoi(pidStr);

        if (strcmp(command, "publisher") == 0)
        {
            addPublisher(pid);
            printf("Publisher %d connected\n", pid);
        }
        else if (strcmp(command, "publisher_exit") == 0)
        {
            removePublisher(pid);
            printf("Publisher %d disconnected\n", pid);
        }
        else if (strcmp(command, "subscribe") == 0)
        {
            if (topic != NULL)
            {
                addSubscriber(pid, topic);

                printf("Subscriber %d subscribed to \"%s\"\n",
                       pid,
                       topic);
            }
        }
        else if (strcmp(command, "unsubscribe") == 0)
        {
            if (topic != NULL)
            {
                removeSubscriber(pid, topic);

                printf("Subscriber %d unsubscribed from \"%s\"\n",
                       pid,
                       topic);
            }
        }
                else if (strcmp(command, "send") == 0)
        {
            char *payload = strtok(NULL, "");

            if (payload == NULL)
                payload = "";

            addPublisher(pid);

            if (topic != NULL)
            {
                printf("Publisher %d -> topic \"%s\"\n",
                       pid,
                       topic);

                forwardMessage(pid,
                               topic,
                               payload);
            }
        }
    }

    printf("\nStopping broker...\n");

    Publisher *pub = publishers;

    while (pub != NULL)
    {
        kill(pub->pid, SIGINT);
        pub = pub->next;
    }

    Subscriber *sub = subscribers;

    while (sub != NULL)
    {
        kill(sub->pid, SIGINT);
        sub = sub->next;
    }

    time_t start = time(NULL);

    while (time(NULL) - start < SHUTDOWN_TIMEOUT)
    {
        if (publishers == NULL && subscribers == NULL)
            break;

        Message msg;

        if (receiveMessage(qid, 1, &msg) == -1)
        {
            struct timespec ts = {0, 100000000};
            nanosleep(&ts, NULL);
            continue;
        }

        char *command = strtok(msg.text, ",");
        char *pidStr = strtok(NULL, ",");
        char *topic = strtok(NULL, ",");

        if (command == NULL || pidStr == NULL)
            continue;

        pid_t pid = (pid_t)atoi(pidStr);

        if (strcmp(command, "publisher_exit") == 0)
        {
            removePublisher(pid);
        }
        else if (strcmp(command, "unsubscribe") == 0)
        {
            if (topic != NULL)
                removeSubscriber(pid, topic);
        }
    }

    msgctl(qid, IPC_RMID, NULL);

    while (publishers != NULL)
    {
        Publisher *tmp = publishers;
        publishers = publishers->next;
        free(tmp);
    }

    while (subscribers != NULL)
    {
        Subscriber *tmp = subscribers;
        subscribers = subscribers->next;
        free(tmp);
    }

    printf("Queue removed.\n");
    printf("Broker stopped.\n");
}