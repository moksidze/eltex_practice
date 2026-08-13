#include "sniffer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <time.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <net/if.h>
#include <net/ethernet.h>
#include <netpacket/packet.h>

#include <netinet/ip.h>
#include <netinet/udp.h>

static volatile sig_atomic_t capture_running = 1;

static void signal_handler(int signal)
{
    (void)signal;
    capture_running = 0;
}

static void print_mac(const unsigned char *mac)
{
    printf("%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

static void print_payload(const unsigned char *data, int length)
{
    if (length <= 0)
    {
        printf("(нет данных)");
        return;
    }

    int max_length = length;

    if (max_length > 512)
        max_length = 512;

    for (int i = 0; i < max_length; i++)
    {
        unsigned char c = data[i];

        if (c >= 32 && c <= 126)
            putchar(c);
        else
            putchar('.');
    }

    if (length > max_length)
        printf("...");
}

static void print_packet(
    const unsigned char *packet,
    ssize_t packet_length,
    const struct timeval *start_time,
    const struct ether_header *ether_header,
    const struct iphdr *ip_header,
    const struct udphdr *udp_header)
{
    struct timeval current_time;

    gettimeofday(&current_time, NULL);

    long seconds = current_time.tv_sec - start_time->tv_sec;

    long microseconds = current_time.tv_usec - start_time->tv_usec;

    if (microseconds < 0)
    {
        seconds--;
        microseconds += 1000000;
    }

    int ip_header_length = ip_header->ihl * 4;

    int udp_header_length = sizeof(struct udphdr);

    const unsigned char *payload = packet + sizeof(struct ether_header) + ip_header_length + udp_header_length;

    int ip_total_length = ntohs(ip_header->tot_len);

    int payload_length = ip_total_length - ip_header_length - udp_header_length;

    if (payload_length < 0)
        payload_length = 0;

    printf("\n========================================\n");

    printf("Время с начала захвата: %ld.%06ld с\n", seconds, microseconds);

    printf("Размер Ethernet кадра: %zd байт\n", packet_length);

    printf("MAC отправителя: ");
    print_mac(ether_header->ether_shost);
    printf("\n");

    printf("MAC получателя: ");
    print_mac(ether_header->ether_dhost);
    printf("\n");

    struct in_addr source_ip;
    struct in_addr destination_ip;

    source_ip.s_addr = ip_header->saddr;
    destination_ip.s_addr = ip_header->daddr;

    printf("IP отправителя: %s\n", inet_ntoa(source_ip));

    printf("IP получателя: %s\n", inet_ntoa(destination_ip));

    printf("UDP порт отправителя: %u\n", ntohs(udp_header->source));

    printf("UDP порт получателя: %u\n", ntohs(udp_header->dest));

    printf("Размер UDP данных: %d байт\n", payload_length);

    printf("Данные: ");

    if (payload_length > 0)
        print_payload(payload, payload_length);
    else
        printf("(нет данных)");

    printf("\n");

    printf("========================================\n");
}

static int packet_matches_filter(const struct udphdr *udp_header, FilterType filter)
{
    unsigned short source_port = ntohs(udp_header->source);

    unsigned short destination_port = ntohs(udp_header->dest);

    if (filter == FILTER_CHAT)
    {
        if (source_port == CHAT_PORT ||
            destination_port == CHAT_PORT)
        {
            return 1;
        }
    }

    if (filter == FILTER_DNS)
    {
        if (source_port == DNS_PORT ||
            destination_port == DNS_PORT)
        {
            return 1;
        }
    }

    return 0;
}

int start_capture(const char *interface_name, FilterType filter)
{
    int socket_fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));

    if (socket_fd == -1)
    {
        perror("socket");
        return -1;
    }

    struct ifreq interface_request;

    memset(&interface_request, 0, sizeof(interface_request));

    strncpy(interface_request.ifr_name, interface_name, IFNAMSIZ - 1);

    if (ioctl(socket_fd, SIOCGIFINDEX, &interface_request) == -1)
    {
        perror("ioctl(SIOCGIFINDEX)");
        close(socket_fd);
        return -1;
    }

    struct sockaddr_ll socket_address;

    memset(&socket_address, 0, sizeof(socket_address));

    socket_address.sll_family = AF_PACKET;

    socket_address.sll_protocol = htons(ETH_P_ALL);

    socket_address.sll_ifindex = interface_request.ifr_ifindex;

    if (bind(socket_fd, (struct sockaddr *)&socket_address, sizeof(socket_address)) == -1)
    {
        perror("bind");
        close(socket_fd);
        return -1;
    }

    struct sigaction signal_action;

    memset(&signal_action, 0, sizeof(signal_action));

    signal_action.sa_handler = signal_handler;

    sigemptyset(&signal_action.sa_mask);

    if (sigaction(SIGINT, &signal_action, NULL) == -1)
    {
        perror("sigaction");
        close(socket_fd);
        return -1;
    }

    struct timeval start_time;

    gettimeofday(&start_time, NULL);

    capture_running = 1;

    printf("\n");
    printf("========================================\n");
    printf("Начало захвата UDP-пакетов\n");
    printf("Интерфейс: %s\n", interface_name);

    if (filter == FILTER_CHAT)
        printf("Фильтр: UDP чат, порт %d\n", CHAT_PORT);
    else
        printf("Фильтр: DNS, UDP порт %d\n", DNS_PORT);

    printf("Для остановки нажмите Ctrl+C\n");
    printf("========================================\n");

    unsigned char buffer[MAX_PACKET_SIZE];

    while (capture_running)
    {
        ssize_t packet_length = recvfrom(socket_fd, buffer, sizeof(buffer), 0, NULL, NULL);

        if (packet_length == -1)
        {
            if (errno == EINTR)
                continue;

            perror("recvfrom");
            break;
        }

        if (packet_length < (ssize_t)sizeof(struct ether_header))
        {
            continue;
        }

        struct ether_header *ether_header = (struct ether_header *)buffer;

        if (ntohs(ether_header->ether_type)
            != ETHERTYPE_IP)
        {
            continue;
        }

        if (packet_length < (ssize_t)(sizeof(struct ether_header) + sizeof(struct iphdr)))
        {
            continue;
        }

        struct iphdr *ip_header = (struct iphdr *)(buffer + sizeof(struct ether_header));

        if (ip_header->version != 4)
            continue;

        if (ip_header->protocol != IPPROTO_UDP)
            continue;

        int ip_header_length = ip_header->ihl * 4;

        if (ip_header_length < 20)
            continue;

        if (packet_length < (ssize_t)(sizeof(struct ether_header) + ip_header_length + sizeof(struct udphdr)))
        {
            continue;
        }

        struct udphdr *udp_header = (struct udphdr *)(buffer + sizeof(struct ether_header) + ip_header_length);

        if (!packet_matches_filter(udp_header, filter))
        {
            continue;
        }

        print_packet(buffer, packet_length, &start_time, ether_header, ip_header, udp_header);
    }

    printf("\n");
    printf("Захват завершён.\n");

    close(socket_fd);

    return 0;
}