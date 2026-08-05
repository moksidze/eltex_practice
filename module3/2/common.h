#ifndef COMMON_H
#define COMMON_H

#include <sys/types.h>

#define QUEUE_KEY 0x12345

#define MAX_TEXT 512
#define MAX_TOPIC 64
#define MAX_TOPICS 10

typedef struct
{
    long mtype;
    char text[MAX_TEXT];
} Message;

int getQueue(int create);

int sendMessage(int qid, long type, const char *text);

int receiveMessage(int qid, long type, Message *msg);

#endif