#include "common.h"

#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>

int getQueue(int create)
{
    int flags = 0666;

    if (create)
        flags |= IPC_CREAT | IPC_EXCL;

    int qid = msgget(QUEUE_KEY, flags);

    if (qid == -1)
    {
        perror("msgget");
    }

    return qid;
}

int sendMessage(int qid, long type, const char *text)
{
    Message msg;

    msg.mtype = type;

    memset(msg.text, 0, sizeof(msg.text));
    strncpy(msg.text, text, MAX_TEXT - 1);

    if (msgsnd(qid, &msg, sizeof(msg.text), 0) == -1)
    {
        perror("msgsnd");
        return -1;
    }

    return 0;
}

int receiveMessage(int qid, long type, Message *msg)
{
    int size = msgrcv(qid, msg, sizeof(msg->text), type, 0);

    if (size == -1)
    {
        if (errno != EIDRM && errno != EINTR)
            perror("msgrcv");

        return -1;
    }

    return size;
}