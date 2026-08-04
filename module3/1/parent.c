#include "parent.h"
#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

void parent_process(int argc, char *argv[], int read_fd, int write_fd)
{
    Header header;
    char buffer[BUF_SIZE];
    char ready;

    if (read(read_fd, &ready, 1) != 1)
    {
        fprintf(stderr, "Child is not ready\n");
        return;
    }

    int firstFile = 1;

    if (argc >= 4 && strcmp(argv[1], "-p") == 0)
        firstFile = 3;

    for (int i = firstFile; i < argc; i++)
    {
        int fd = open(argv[i], O_RDONLY);

        if (fd < 0)
        {
            fprintf(stderr, "Cannot open file %s\n", argv[i]);
            continue;
        }

        struct stat st;

        if (fstat(fd, &st) == -1)
        {
            fprintf(stderr, "Cannot get file size %s\n", argv[i]);
            close(fd);
            continue;
        }

        memset(&header, 0, sizeof(Header));

        strncpy(header.filename, argv[i], NAME_SIZE - 1);
        header.filesize = st.st_size;

        if (write(write_fd, &header, sizeof(Header)) != sizeof(Header))
        {
            perror("write header");
            close(fd);
            return;
        }

        int bytes;

        while ((bytes = read(fd, buffer, BUF_SIZE)) > 0)
        {
            if (write(write_fd, buffer, bytes) != bytes)
            {
                perror("write data");
                close(fd);
                return;
            }
        }

        close(fd);

        if (read(read_fd, &ready, 1) != 1)
        {
            fprintf(stderr, "Child disconnected\n");
            return;
        }
    }

    memset(&header, 0, sizeof(Header));
    header.filesize = -1;

    write(write_fd, &header, sizeof(Header));
}