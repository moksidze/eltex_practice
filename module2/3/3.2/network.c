#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include "network.h"

uint32_t ipToInt(const char *ip)
{
    unsigned int a, b, c, d;

    if (sscanf(ip, "%u.%u.%u.%u", &a, &b, &c, &d) != 4)
    {
        printf("Некорректный IP адрес!\n");
        exit(1);
    }

    return (a << 24) | (b << 16) | (c << 8) | d;
}

void intToIp(uint32_t ip)
{
    printf("%u.%u.%u.%u",
           (ip >> 24) & 255,
           (ip >> 16) & 255,
           (ip >> 8) & 255,
           ip & 255);
}