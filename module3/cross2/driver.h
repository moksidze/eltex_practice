#ifndef DRIVER_H
#define DRIVER_H

#include <sys/types.h>

#define MAX_DRIVERS 100
#define BUFFER_SIZE 256

typedef enum
{
    DRIVER_AVAILABLE,
    DRIVER_BUSY
} DriverStatus;

typedef struct
{
    pid_t pid;
    int fd;
    DriverStatus status;
    int remaining;
} Driver;

void driver_process(int fd);

#endif