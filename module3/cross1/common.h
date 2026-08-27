#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#include <netinet/ip.h>
#include <netinet/udp.h>

#define SERVER_PORT 5000
#define BUFFER_SIZE 1024
#define CLOSE_MESSAGE "__CLOSE__"

typedef struct {
    struct iphdr ip;
    struct udphdr udp;
    char data[BUFFER_SIZE];
} Packet;

uint16_t checksum(void *data, int len);

int create_raw_socket(void);

int send_udp_packet(
    int sock,
    const char *src_ip,
    int src_port,
    const char *dst_ip,
    int dst_port,
    const char *data,
    int data_len
);

#endif