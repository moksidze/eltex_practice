#include "driver.h"
#include "ipc.h"
#include "timer.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <poll.h>
#include <time.h>

static volatile sig_atomic_t timer_expired = 0;

static void timer_handler(int signal)
{
    (void)signal;

    timer_expired = 1;
}

static void setup_signal_handler(void)
{
    struct sigaction action;

    memset(&action, 0, sizeof(action));

    action.sa_handler = timer_handler;

    sigemptyset(&action.sa_mask);

    action.sa_flags = 0;

    if (sigaction(SIGRTMIN, &action, NULL) == -1)
    {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
}

static void send_status(int fd, time_t busy_until)
{
    char message[IPC_BUFFER_SIZE];

    if (busy_until == 0)
    {
        snprintf(message, sizeof(message), "Available\n");
    }
    else
    {
        time_t now = time(NULL);
        int remaining = (int)(busy_until - now);

        if (remaining <= 0)
        {
            snprintf(message, sizeof(message), "Available\n");
        }
        else
        {
            snprintf(message, sizeof(message), "Busy %d\n", remaining);
        }
    }

    send_message(fd, message);
}

void driver_process(int fd)
{
    timer_t timer_id;

    setup_signal_handler();

    if (create_driver_timer(&timer_id) == -1)
    {
        perror("timer_create");
        close(fd);
        exit(EXIT_FAILURE);
    }

    time_t busy_until = 0;

    struct pollfd pfd;

    pfd.fd = fd;
    pfd.events = POLLIN;

    while (1)
    {
        if (timer_expired)
        {
            timer_expired = 0;
            busy_until = 0;

            send_message(fd, "Available\n");
        }

        int result = poll(&pfd, 1, -1);

        if (result < 0)
        {
            if (errno == EINTR)
                continue;

            perror("poll");
            break;
        }

        if (pfd.revents & POLLIN)
        {
            char message[IPC_BUFFER_SIZE];

            int received = receive_message(fd, message, sizeof(message));

            if (received <= 0)
                break;

            if (strncmp(message, "TASK ", 5) == 0)
            {
                int seconds = atoi(message + 5);

                if (busy_until != 0)
                {
                    time_t now = time(NULL);

                    int remaining = (int)(busy_until - now);

                    if (remaining > 0)
                    {
                        char response[IPC_BUFFER_SIZE];

                        snprintf(response, sizeof(response), "Busy %d\n", remaining);

                        send_message(fd, response);

                        continue;
                    }

                    busy_until = 0;
                }

                if (seconds <= 0)
                {
                    send_message(fd, "ERROR Invalid timer\n");
                    continue;
                }

                busy_until = time(NULL) + seconds;

                if (start_driver_timer(timer_id, seconds) == -1)
                {
                    perror("timer_settime");
                    break;
                }

                send_message(fd, "OK\n");
            }
            else if (strcmp(message, "STATUS\n") == 0)
            {
                send_status(fd, busy_until);
            }
            else if (strcmp(message, "EXIT\n") == 0)
            {
                break;
            }
            else
            {
                send_message(fd, "ERROR Unknown command\n");
            }
        }

        if (pfd.revents & (POLLHUP | POLLERR | POLLNVAL))
        {
            break;
        }
    }

    stop_driver_timer(timer_id);
    delete_driver_timer(timer_id);

    close(fd);

    exit(EXIT_SUCCESS);
}
