#include <stdio.h>
#include <string.h>

#include "broker.h"
#include "publisher.h"
#include "subscriber.h"

static void usage(const char *prog)
{
    printf("Usage:\n");
    printf("  %s -b\n", prog);
    printf("  %s -p <topic>\n", prog);
    printf("  %s -s <topic1> [topic2] ...\n", prog);
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        usage(argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "-b") == 0)
    {
        brokerRun();
    }
    else if (strcmp(argv[1], "-p") == 0)
    {
        if (argc != 3)
        {
            usage(argv[0]);
            return 1;
        }

        publisherRun(argv[2]);
    }
    else if (strcmp(argv[1], "-s") == 0)
    {
        if (argc < 3)
        {
            usage(argv[0]);
            return 1;
        }

        subscriberRun(argc - 2, &argv[2]);
    }
    else
    {
        usage(argv[0]);
        return 1;
    }

    return 0;
}