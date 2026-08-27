#include "common.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/udp.h>

uint16_t checksum(void *data, int len)
{
    uint32_t sum = 0;
    uint16_t *ptr = data;

    while (len > 1) {
        sum += *ptr++;
        len -= 2;
    }

    if (len == 1) {
        sum += *(uint8_t *)ptr;
    }

    while (sum >> 16) {
        sum = (sum & 0xffff) + (sum >> 16);
    }

    return (uint16_t)(~sum);
}

int create_raw_socket(void)
{
    int sock;

    sock = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);

    if (sock < 0) {
        perror("socket");
        return -1;
    }

    int one = 1;

    if (setsockopt(
            sock,
            IPPROTO_IP,
            IP_HDRINCL,
            &one,
            sizeof(one)) < 0) {

        perror("setsockopt(IP_HDRINCL)");
        close(sock);
        return -1;
    }

    return sock;
}

int send_udp_packet(
    int sock,
    const char *src_ip,
    int src_port,
    const char *dst_ip,
    int dst_port,
    const char *data,
    int data_len)
{
    char packet[BUFFER_SIZE + sizeof(struct iphdr) + sizeof(struct udphdr)];

    memset(packet, 0, sizeof(packet));

    struct iphdr *ip = (struct iphdr *)packet;

    struct udphdr *udp =
        (struct udphdr *)(packet + sizeof(struct iphdr));

    char *payload =
        packet + sizeof(struct iphdr) + sizeof(struct udphdr);

    memcpy(payload, data, data_len);

    ip->ihl = 5;
    ip->version = 4;
    ip->tos = 0;

    ip->tot_len = htons(
        sizeof(struct iphdr) +
        sizeof(struct udphdr) +
        data_len
    );

    ip->id = htons(12345);

    ip->frag_off = 0;

    ip->ttl = 64;

    ip->protocol = IPPROTO_UDP;

    ip->check = 0;

    ip->saddr = inet_addr(src_ip);
    ip->daddr = inet_addr(dst_ip);

    ip->check = checksum(ip, sizeof(struct iphdr));

    udp->source = htons(src_port);
    udp->dest = htons(dst_port);

    udp->len = htons(
        sizeof(struct udphdr) + data_len
    );

    udp->check = 0;

    struct sockaddr_in destination;

    memset(&destination, 0, sizeof(destination));

    destination.sin_family = AF_INET;
    destination.sin_port = htons(dst_port);
    destination.sin_addr.s_addr = inet_addr(dst_ip);

    int packet_len =
        sizeof(struct iphdr) +
        sizeof(struct udphdr) +
        data_len;

    if (sendto(
            sock,
            packet,
            packet_len,
            0,
            (struct sockaddr *)&destination,
            sizeof(destination)) < 0) {

        perror("sendto");
        return -1;
    }

    return 0;
}
