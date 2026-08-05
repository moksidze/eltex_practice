#ifndef BROKER_H
#define BROKER_H

#include "common.h"
#include <sys/types.h>

typedef struct Subscriber
{
    pid_t pid;
    char topic[MAX_TOPIC];
    struct Subscriber *next;
} Subscriber;

typedef struct Publisher
{
    pid_t pid;
    struct Publisher *next;
} Publisher;

void brokerRun(void);

#endif