#include "child.h"
#include "common.h"

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

void child_process(int read_fd, int write_fd)
{
    Header header;
    char buffer[BUF_SIZE];
    char ready = 'R';

    if (write(write_fd, &ready, 1) != 1)
    {
        perror("READY");
        return;
    }

    while (1)
    {
        if (read(read_fd, &header, sizeof(Header)) != sizeof(Header))
            break;

        if (header.filesize == -1)
            break;

        char newname[NAME_SIZE + 10];
        snprintf(newname, sizeof(newname), "%s.copy", header.filename);

        int fd = open(newname, O_WRONLY | O_CREAT | O_TRUNC, 0644);

        if (fd < 0)
        {
            perror(newname);
            return;
        }

        int left = header.filesize;

        while (left > 0)
        {
            int chunk = left > BUF_SIZE ? BUF_SIZE : left;

            int n = read(read_fd, buffer, chunk);

            if (n <= 0)
                break;

            write(fd, buffer, n);

            left -= n;
        }

        close(fd);

        write(write_fd, &ready, 1);
    }
}