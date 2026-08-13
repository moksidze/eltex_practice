#include "common.h"

#include <unistd.h>
#include <errno.h>
#include <stddef.h>

int send_all(int sockfd, const void *buffer, size_t size)
{
    const char *data = buffer;

    size_t total_sent = 0;

    while (total_sent < size)
    {
        ssize_t sent = send(sockfd, data + total_sent, size - total_sent, 0);

        if (sent == -1)
        {
            if (errno == EINTR)
            {
                continue;
            }

            return -1;
        }

        if (sent == 0)
        {
            return -1;
        }

        total_sent += sent;
    }

    return 0;
}

int recv_all(int sockfd, void *buffer, size_t size)
{
    char *data = buffer;

    size_t total_received = 0;

    while (total_received < size)
    {
        ssize_t received = recv(sockfd, data + total_received, size - total_received, 0);

        if (received == -1)
        {
            if (errno == EINTR)
            {
                continue;
            }

            return -1;
        }

        if (received == 0)
        {
            return -1;
        }

        total_received += received;
    }

    return 0;
}