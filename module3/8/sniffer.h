#ifndef SNIFFER_H
#define SNIFFER_H

#define CHAT_PORT 5000
#define DNS_PORT 53

#define MAX_PACKET_SIZE 65536

typedef enum
{
    FILTER_CHAT = 1,
    FILTER_DNS = 2
} FilterType;

int start_capture(const char *interface_name, FilterType filter);

#endif