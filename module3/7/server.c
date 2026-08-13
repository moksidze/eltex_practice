#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <poll.h>

typedef struct
{
    int fd;
    char username[USERNAME_SIZE];
} Client;

static void init_clients(Client clients[])
{
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        clients[i].fd = -1;
        clients[i].username[0] = '\0';
    }
}

static int find_free_client(Client clients[])
{
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i].fd == -1)
        {
            return i;
        }
    }

    return -1;
}

static void broadcast_message(Client clients[], int exclude_fd, MessageHeader *header, const void *data)
{
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i].fd == -1)
        {
            continue;
        }

        if (clients[i].fd == exclude_fd)
        {
            continue;
        }

        if (send_all(clients[i].fd, header, sizeof(*header)) == -1)
        {
            continue;
        }

        if (header->size > 0)
        {
            if (send_all(clients[i].fd, data, header->size) == -1)
            {
                continue;
            }
        }
    }
}

static int create_server_socket(void)
{
    int server_fd;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd == -1)
    {
        perror("socket");
        return -1;
    }

    int reuse = 1;

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1)
    {
        perror("setsockopt");

        close(server_fd);

        return -1;
    }

    struct sockaddr_in address;

    memset(&address, 0, sizeof(address));

    address.sin_family = AF_INET;

    address.sin_addr.s_addr = htonl(INADDR_ANY);

    address.sin_port = htons(SERVER_PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == -1)
    {
        perror("bind");

        close(server_fd);

        return -1;
    }

    if (listen(server_fd, MAX_CLIENTS) == -1)
    {
        perror("listen");

        close(server_fd);

        return -1;
    }

    return server_fd;
}

int main(void)
{
    Client clients[MAX_CLIENTS];

    init_clients(clients);

    int server_fd = create_server_socket();

    if (server_fd == -1)
    {
        return EXIT_FAILURE;
    }

    printf("TCP сервер запущен.\n");
    printf("Порт: %d\n", SERVER_PORT);
    printf("Ожидание подключений...\n");

    struct pollfd fds[MAX_CLIENTS + 1];


    while (1)
    {
        memset(fds, 0, sizeof(fds));

        fds[0].fd = server_fd;
        fds[0].events = POLLIN;

        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            fds[i + 1].fd = clients[i].fd;

            if (clients[i].fd != -1)
            {
                fds[i + 1].events = POLLIN;
            }
        }

        int result = poll(fds, MAX_CLIENTS + 1, -1);

        if (result == -1)
        {
            if (errno == EINTR)
            {
                continue;
            }

            perror("poll");

            break;
        }

        if (fds[0].revents & POLLIN)
        {
            struct sockaddr_in client_address;

            socklen_t client_address_len = sizeof(client_address);

            int client_fd = accept(server_fd, (struct sockaddr *)&client_address, &client_address_len);

            if (client_fd == -1)
            {
                perror("accept");
            }
            else
            {
                int index = find_free_client(clients);

                if (index == -1)
                {
                    printf("Слишком много клиентов.\n");

                    close(client_fd);
                }
                else
                {
                    clients[index].fd = client_fd;

                    clients[index].username[0] = '\0';

                    printf("Новое TCP-подключение: fd=%d\n", client_fd);
                }
            }
        }

        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            if (clients[i].fd == -1)
            {
                continue;
            }

            short events = fds[i + 1].revents;

            if (events & (POLLHUP | POLLERR | POLLNVAL))
            {
                printf("Клиент отключился: %s\n", clients[i].username);

                if (clients[i].username[0] != '\0')
                {
                    MessageHeader header;

                    memset(&header, 0, sizeof(header));

                    header.type = MSG_LEAVE;

                    header.size = 0;

                    strncpy(header.username, clients[i].username, USERNAME_SIZE - 1);

                    broadcast_message(clients, clients[i].fd, &header, NULL);
                }

                close(clients[i].fd);

                clients[i].fd = -1;

                clients[i].username[0] = '\0';

                continue;
            }

            if (!(events & POLLIN))
            {
                continue;
            }

            MessageHeader header;

            if (recv_all(clients[i].fd, &header, sizeof(header)) == -1)
            {
                printf("Ошибка получения данных от клиента.\n");

                close(clients[i].fd);

                clients[i].fd = -1;

                clients[i].username[0] = '\0';

                continue;
            }

            if (header.size > BUFFER_SIZE &&
                header.type != MSG_FILE)
            {
                printf("Слишком большое сообщение.\n");

                close(clients[i].fd);

                clients[i].fd = -1;

                clients[i].username[0] = '\0';

                continue;
            }

            if (header.type == MSG_JOIN)
            {
                strncpy(clients[i].username, header.username, USERNAME_SIZE - 1);

                clients[i].username[USERNAME_SIZE - 1] = '\0';

                printf("Клиент подключился: %s\n", clients[i].username);

                broadcast_message(clients, clients[i].fd, &header, NULL);

                continue;
            }

            if (header.type == MSG_CHAT)
            {
                char *data = malloc(header.size + 1);

                if (data == NULL)
                {
                    perror("malloc");

                    break;
                }

                if (recv_all(clients[i].fd, data, header.size) == -1)
                {
                    free(data);

                    close(clients[i].fd);

                    clients[i].fd = -1;

                    clients[i].username[0] = '\0';

                    continue;
                }

                data[header.size] = '\0';

                printf("[%s]: %s\n", clients[i].username, data);

                broadcast_message(clients, clients[i].fd, &header, data);

                free(data);
            }

            else if (header.type == MSG_FILE)
            {
                printf("Получение файла \"%s\" от %s ""(%u байт)\n", header.filename, clients[i].username, header.size);

                unsigned char *file_data = malloc(header.size);

                if (file_data == NULL)
                {
                    perror("malloc");

                    break;
                }

                if (recv_all(clients[i].fd, file_data, header.size) == -1)
                {
                    free(file_data);

                    close(clients[i].fd);

                    clients[i].fd = -1;

                    clients[i].username[0] = '\0';

                    continue;
                }

                broadcast_message(clients, clients[i].fd, &header, file_data);

                printf("Файл \"%s\" отправлен ""остальным клиентам.\n", header.filename);

                free(file_data);
            }
        }
    }

    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i].fd != -1)
        {
            close(clients[i].fd);
        }
    }

    close(server_fd);

    return EXIT_SUCCESS;
}