#include "ipc.h"

#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

int create_socket_pair(int sv[2])
{
    return socketpair(AF_UNIX, SOCK_STREAM, 0, sv);
}

int send_message(int fd, const char *message)
{
    size_t len = strlen(message);
    size_t sent = 0;

    while (sent < len)
    {
        ssize_t result = send(fd, message + sent, len - sent, 0);

        if (result < 0)
        {
            if (errno == EINTR)
                continue;

            return -1;
        }

        sent += (size_t)result;
    }

    return 0;
}

int receive_message(int fd, char *buffer, size_t size)
{
    if (size == 0)
        return -1;

    ssize_t received = recv(fd, buffer, size - 1, 0);

    if (received < 0)
    {
        if (errno == EINTR)
            return 0;

        return -1;
    }

    if (received == 0)
        return 0;

    buffer[received] = '\0';

    return (int)received;
}