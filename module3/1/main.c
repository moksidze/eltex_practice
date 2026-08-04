#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include "parent.h"
#include "child.h"

int main(int argc, char *argv[])
{
    int p2c[2];
    int c2p[2];

    int use_fifo = 0;
    char *fifo_name = NULL;

    if (argc >= 3 && strcmp(argv[1], "-p") == 0)
    {
        use_fifo = 1;
        fifo_name = argv[2];
    }

    if (!use_fifo)
    {
        if (pipe(p2c) == -1)
        {
            perror("pipe");
            return EXIT_FAILURE;
        }

        if (pipe(c2p) == -1)
        {
            perror("pipe");
            return EXIT_FAILURE;
        }
    }
    else
    {
        mkfifo(fifo_name, 0666);

        if (pipe(c2p) == -1)
        {
            perror("pipe");
            return EXIT_FAILURE;
        }
    }

    pid_t pid = fork();

    if (pid < 0)
    {
        perror("fork");
        return EXIT_FAILURE;
    }

    if (pid == 0)
    {
        if (!use_fifo)
        {
            close(p2c[1]);
            close(c2p[0]);

            child_process(p2c[0], c2p[1]);

            close(p2c[0]);
            close(c2p[1]);
        }
        else
        {
            close(c2p[0]);

            int fifo_fd = open(fifo_name, O_RDONLY);

            child_process(fifo_fd, c2p[1]);

            close(fifo_fd);
            close(c2p[1]);
        }
    }
    else
    {
        if (!use_fifo)
        {
            close(p2c[0]);
            close(c2p[1]);

            parent_process(argc,
                           argv,
                           c2p[0],
                           p2c[1]);

            close(c2p[0]);
            close(p2c[1]);
        }
        else
        {
            close(c2p[1]);

            int fifo_fd = open(fifo_name, O_WRONLY);

            parent_process(argc,
                           argv,
                           c2p[0],
                           fifo_fd);

            close(c2p[0]);
            close(fifo_fd);

            unlink(fifo_name);
        }

        wait(NULL);
    }

    return 0;
}