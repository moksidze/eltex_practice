#include "chat.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int create_socket(void)
{
    int sockfd;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd == -1)
    {
        perror("socket");
        return -1;
    }

    int reuse = 1;

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1)
    {
        perror("setsockopt SO_REUSEADDR");
        close(sockfd);
        return -1;
    }

    int broadcast = 1;

    if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast)) == -1)
    {
        perror("setsockopt SO_BROADCAST");
        close(sockfd);
        return -1;
    }

    struct sockaddr_in address;

    memset(&address, 0, sizeof(address));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr *)&address, sizeof(address)) == -1)
    {
        perror("bind");
        close(sockfd);
        return -1;
    }

    return sockfd;
}


int send_broadcast(int sockfd, const char *message)
{
    struct sockaddr_in broadcast_address;

    memset(&broadcast_address, 0, sizeof(broadcast_address));

    broadcast_address.sin_family = AF_INET;
    broadcast_address.sin_port = htons(PORT);

    if (inet_pton(AF_INET, BROADCAST_IP, &broadcast_address.sin_addr) <= 0)
    {
        perror("inet_pton");
        return -1;
    }

    if (sendto(sockfd, message, strlen(message), 0, (struct sockaddr *)&broadcast_address, sizeof(broadcast_address)) == -1)
    {
        perror("sendto");
        return -1;
    }

    return 0;
}


int send_message(int sockfd, const char *message)
{
    return send_broadcast(sockfd, message);
}


int receive_message(int sockfd, char *buffer, int size)
{
    struct sockaddr_in sender;
    socklen_t sender_len = sizeof(sender);

    int bytes_received = recvfrom(sockfd, buffer, size - 1, 0, (struct sockaddr *)&sender, &sender_len);

    if (bytes_received == -1)
    {
        perror("recvfrom");
        return -1;
    }

    buffer[bytes_received] = '\0';

    return bytes_received;
}


void close_socket(int sockfd)
{
    if (sockfd >= 0)
    {
        close(sockfd);
    }
}