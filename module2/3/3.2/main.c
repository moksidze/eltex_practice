#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include "network.h"

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        printf("Использование:\n");
        printf("%s <IP шлюза> <маска> <количество пакетов>\n", argv[0]);
        return 1;
    }

    uint32_t gateway = ipToInt(argv[1]);
    uint32_t mask = ipToInt(argv[2]);

    int N = atoi(argv[3]);

    srand(time(NULL));

    uint32_t network = gateway & mask;

    int local = 0;
    int external = 0;

    printf("Сеть шлюза: ");
    intToIp(network);
    printf("\n\n");

    for (int i = 0; i < N; i++)
    {
        uint32_t ip =
            ((uint32_t)rand() << 16) ^ (uint32_t)rand();

        uint32_t packetNetwork = ip & mask;

        printf("Пакет %2d -> ", i + 1);
        intToIp(ip);

        if (packetNetwork == network)
        {
            printf("  (своя подсеть)\n");
            local++;
        }
        else
        {
            printf("  (другая сеть)\n");
            external++;
        }
    }

    printf("\n========== Статистика ==========\n");

    printf("Своя подсеть : %d (%.2f%%)\n",
           local,
           100.0 * local / N);

    printf("Другие сети  : %d (%.2f%%)\n",
           external,
           100.0 * external / N);

    return 0;
}