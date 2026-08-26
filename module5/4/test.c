#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define DEVICE_PATH "/dev/mychardev"
#define BUFFER_SIZE 1024

int main(void)
{
    int fd;
    char message[] = "Hello from userspace!";
    char buffer[BUFFER_SIZE];

    ssize_t bytes_written;
    ssize_t bytes_read;

    fd = open(DEVICE_PATH, O_RDWR);

    if (fd < 0) {
        perror("open");
        return EXIT_FAILURE;
    }

    printf("Device opened: %s\n", DEVICE_PATH);

    bytes_written = write(fd, message, strlen(message));

    if (bytes_written < 0) {
        perror("write");
        close(fd);
        return EXIT_FAILURE;
    }

    printf("Written to device: %zd bytes\n", bytes_written);

    memset(buffer, 0, sizeof(buffer));

    bytes_read = read(fd, buffer, sizeof(buffer) - 1);

    if (bytes_read < 0) {
        perror("read");
        close(fd);
        return EXIT_FAILURE;
    }

    buffer[bytes_read] = '\0';

    printf("Read from device: %s\n", buffer);

    close(fd);

    printf("Device closed\n");

    return EXIT_SUCCESS;
}
