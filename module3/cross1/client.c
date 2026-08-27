#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/udp.h>

static int client_socket = -1;

static char server_ip[INET_ADDRSTRLEN];
static char client_ip[INET_ADDRSTRLEN];

static int client_port = 5001;

void handle_signal(int signal)
{
    if (signal == SIGINT) {
        printf("\nClosing client...\n");
        if (client_socket != -1) {
            send_udp_packet(
                client_socket,
                client_ip,
                client_port,
                server_ip,
                SERVER_PORT,
                CLOSE_MESSAGE,
                strlen(CLOSE_MESSAGE)
            );
            close(client_socket);
        }
        exit(0);
    }
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        printf("Usage: %s <server_ip> <client_port>\n", argv[0]);
        return 1;
    }

    strncpy(server_ip, argv[1], sizeof(server_ip) - 1);
    client_port = atoi(argv[2]);

    printf("Enter client IP: ");

    if (scanf("%15s", client_ip) != 1) {
        return 1;
    }

    getchar();

    signal(SIGINT, handle_signal);
    client_socket = create_raw_socket();

    if (client_socket < 0) {
        return 1;
    }

    printf("\nRaw UDP client started\n");
    printf("Server: %s:%d\n", server_ip, SERVER_PORT);
    printf("Client: %s:%d\n\n", client_ip, client_port);

    char message[BUFFER_SIZE];

    while (1) {
        printf("> ");

        if (fgets(message, sizeof(message), stdin) == NULL) {
            break;
        }

        message[strcspn(message, "\n")] = '\0';

        if (strlen(message) == 0) {
            continue;
        }

        send_udp_packet(
            client_socket,
            client_ip,
            client_port,
            server_ip,
            SERVER_PORT,
            message,
            strlen(message)
        );

        char buffer[BUFFER_SIZE + sizeof(struct iphdr)];

        ssize_t received = recvfrom(
            client_socket,
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

        struct iphdr *ip = (struct iphdr *)buffer;

        if (ip->protocol != IPPROTO_UDP) {
            continue;
        }

        struct udphdr *udp = (struct udphdr *)(buffer + ip->ihl * 4);

        if (ip->saddr != inet_addr(server_ip)) {
            continue;
        }

        if (ntohs(udp->source) != SERVER_PORT) {
            continue;
        }

        char *data = buffer + ip->ihl * 4 + sizeof(struct udphdr);

        int data_len = ntohs(udp->len) - sizeof(struct udphdr);

        if (data_len >= BUFFER_SIZE) {
            data_len = BUFFER_SIZE - 1;
        }

        data[data_len] = '\0';

        printf("Server: %s\n", data);
    }

    if (client_socket != -1) {
        send_udp_packet(
            client_socket,
            client_ip,
            client_port,
            server_ip,
            SERVER_PORT,
            CLOSE_MESSAGE,
            strlen(CLOSE_MESSAGE)
        );

        close(client_socket);
    }

    return 0;
}
