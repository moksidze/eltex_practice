#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>

#define SERVER_PORT 5000

#define MAX_CLIENTS 100
#define BUFFER_SIZE 4096
#define USERNAME_SIZE 64
#define FILENAME_SIZE 256

typedef enum
{
    MSG_JOIN = 1,
    MSG_CHAT,
    MSG_FILE,
    MSG_LEAVE
} MessageType;

typedef struct
{
    uint32_t type;
    uint32_t size;
    char username[USERNAME_SIZE];
    char filename[FILENAME_SIZE];
} MessageHeader;

int send_all(int sockfd, const void *buffer, size_t size);

int recv_all(int sockfd, void *buffer, size_t size);

#endif