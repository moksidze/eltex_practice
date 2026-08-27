#ifndef IPC_H
#define IPC_H

#include <stddef.h>

#define IPC_BUFFER_SIZE 256

int create_socket_pair(int sv[2]);

int send_message(int fd, const char *message);

int receive_message(int fd, char *buffer, size_t size);

#endif