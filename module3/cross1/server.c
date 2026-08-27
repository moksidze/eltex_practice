#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/udp.h>

typedef struct Client {
    uint32_t ip;
    uint16_t port;
    int counter;

    struct Client *next;
} Client;

static Client *clients = NULL;
static int server_socket = -1;

static const char *server_ip = "192.168.100.1";

Client *find_client(uint32_t ip, uint16_t port)
{
    Client *current = clients;

    while (current != NULL) {

        if (current->ip == ip &&
            current->port == port) {

            return current;
        }

        current = current->next;
    }

    return NULL;
}

Client *add_client(uint32_t ip, uint16_t port)
{
    Client *client = malloc(sizeof(Client));

    if (client == NULL) {
        perror("malloc");
        return NULL;
    }

    client->ip = ip;
    client->port = port;
    client->counter = 0;

    client->next = clients;
    clients = client;

    return client;
}

void remove_client(uint32_t ip, uint16_t port)
{
    Client *current = clients;
    Client *previous = NULL;

    while (current != NULL) {

        if (current->ip == ip &&
            current->port == port) {

            if (previous == NULL) {
                clients = current->next;
            } else {
                previous->next = current->next;
            }

            free(current);

            printf("Client removed\n");

            return;
        }

        previous = current;
        current = current->next;
    }
}

void handle_signal(int signal)
{
    if (signal == SIGINT) {

        printf("\nServer shutting down...\n");

        Client *current = clients;

        while (current != NULL) {
            Client *next = current->next;
            free(current);
            current = next;
        }

        clients = NULL;

        if (server_socket != -1) {
            close(server_socket);
        }

        exit(0);
    }
}

int main(void)
{
    signal(SIGINT, handle_signal);

    server_socket = create_raw_socket();

    if (server_socket < 0) {
        return 1;
    }

    printf("Raw UDP server started\n");
    printf("Listening on port %d...\n", SERVER_PORT);

    char buffer[BUFFER_SIZE + sizeof(struct iphdr)];

    while (1) {

        ssize_t received = recvfrom(
            server_socket,
            buffer,
            sizeof(buffer),
            0,
            NULL,
            NULL
        );

        if (received < 0) {
            perror("recvfrom");
            continue;
        }

        struct iphdr *ip =
            (struct iphdr *)buffer;

        if (ip->protocol != IPPROTO_UDP) {
            continue;
        }

        struct udphdr *udp =
            (struct udphdr *)(buffer + ip->ihl * 4);

        int ip_header_len = ip->ihl * 4;

        char *data =
            buffer +
            ip_header_len +
            sizeof(struct udphdr);

        int data_len =
            ntohs(udp->len) -
            sizeof(struct udphdr);

        if (data_len <= 0 ||
            data_len >= BUFFER_SIZE) {

            continue;
        }

        data[data_len] = '\0';

        char client_ip[INET_ADDRSTRLEN];

        inet_ntop(
            AF_INET,
            &ip->saddr,
            client_ip,
            sizeof(client_ip)
        );

        uint16_t client_port =
            ntohs(udp->source);

        printf(
            "Received from %s:%d: %s\n",
            client_ip,
            client_port,
            data
        );

        if (strcmp(data, CLOSE_MESSAGE) == 0) {

            remove_client(
                ip->saddr,
                udp->source
            );

            continue;
        }

        Client *client =
            find_client(
                ip->saddr,
                udp->source
            );

        if (client == NULL) {

            client =
                add_client(
                    ip->saddr,
                    udp->source
                );

            if (client == NULL) {
                continue;
            }
        }

        client->counter++;

        char response[BUFFER_SIZE];

        snprintf(
            response,
            sizeof(response),
            "%s %d",
            data,
            client->counter
        );

        send_udp_packet(
            server_socket,
            server_ip,
            SERVER_PORT,
            client_ip,
            client_port,
            response,
            strlen(response)
        );
    }

    return 0;
}
