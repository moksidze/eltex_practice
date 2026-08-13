#ifndef CHAT_H
#define CHAT_H

#include <netinet/in.h>

#define PORT 5000
#define BUFFER_SIZE 1024
#define BROADCAST_IP "255.255.255.255"

int create_socket(void);

int send_broadcast(int sockfd, const char *message);

int send_message(int sockfd, const char *message);

int receive_message(int sockfd, char *buffer, int size);

void close_socket(int sockfd);

#endif