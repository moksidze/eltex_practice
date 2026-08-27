#include "driver.h"
#include "ipc.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <poll.h>
#include <sys/wait.h>

static Driver drivers[MAX_DRIVERS];
static int driver_count = 0;

static void init_drivers(void)
{
    for (int i = 0; i < MAX_DRIVERS; i++)
    {
        drivers[i].pid = -1;
        drivers[i].fd = -1;
        drivers[i].status = DRIVER_AVAILABLE;
        drivers[i].remaining = 0;
    }
}

static int find_driver(pid_t pid)
{
    for (int i = 0; i < driver_count; i++)
    {
        if (drivers[i].pid == pid)
            return i;
    }

    return -1;
}

static void create_driver(void)
{
    if (driver_count >= MAX_DRIVERS)
    {
        printf("Maximum number of drivers reached\n");
        return;
    }

    int sv[2];

    if (create_socket_pair(sv) == -1)
    {
        perror("socketpair");
        return;
    }

    pid_t pid = fork();

    if (pid == -1)
    {
        perror("fork");
        close(sv[0]);
        close(sv[1]);
        return;
    }

    if (pid == 0)
    {
        close(sv[0]);
        driver_process(sv[1]);
        exit(EXIT_SUCCESS);
    }

    close(sv[1]);

    drivers[driver_count].pid = pid;
    drivers[driver_count].fd = sv[0];
    drivers[driver_count].status = DRIVER_AVAILABLE;
    drivers[driver_count].remaining = 0;

    driver_count++;

    printf("Driver created: PID %d\n", pid);
}

static void update_driver_status(int index)
{
    char buffer[IPC_BUFFER_SIZE];

    if (receive_message(drivers[index].fd, buffer, sizeof(buffer)) <= 0)
    {
        return;
    }

    if (strncmp(buffer, "Available", 9) == 0)
    {
        drivers[index].status = DRIVER_AVAILABLE;
        drivers[index].remaining = 0;

        printf("Driver %d: Available\n", drivers[index].pid);
    }
    else if (strncmp(buffer, "Busy ", 5) == 0)
    {
        int remaining = atoi(buffer + 5);

        drivers[index].status = DRIVER_BUSY;
        drivers[index].remaining = remaining;

        printf("Driver %d: Busy %d\n", drivers[index].pid, remaining);
    }
    else
    {
        printf("Driver %d: %s", drivers[index].pid, buffer);
    }
}

static void send_task(pid_t pid, int seconds)
{
    int index = find_driver(pid);

    if (index == -1)
    {
        printf("Driver %d not found\n", pid);
        return;
    }

    char message[IPC_BUFFER_SIZE];

    snprintf(message, sizeof(message), "TASK %d\n", seconds);

    if (send_message(drivers[index].fd, message) == -1)
    {
        perror("send");
        return;
    }
}

static void get_status(pid_t pid)
{
    int index = find_driver(pid);

    if (index == -1)
    {
        printf("Driver %d not found\n", pid);
        return;
    }

    if (send_message(drivers[index].fd, "STATUS\n") == -1)
    {
        perror("send");
    }
}

static void get_drivers(void)
{
    printf("\n");
    printf("%-10s %-15s\n", "PID", "STATUS");
    printf("-------------------------\n");

    for (int i = 0; i < driver_count; i++)
    {
        if (drivers[i].status == DRIVER_AVAILABLE)
        {
            printf("%-10d Available\n", drivers[i].pid);
        }
        else
        {
            printf("%-10d Busy %d\n", drivers[i].pid, drivers[i].remaining);
        }
    }

    printf("\n");
}

static void process_command(char *command)
{
    char *token = strtok(command, " \n");

    if (token == NULL)
        return;

    if (strcmp(token, "create_driver") == 0)
    {
        create_driver();
    }
    else if (strcmp(token, "send_task") == 0)
    {
        char *pid_string = strtok(NULL, " \n");
        char *timer_string = strtok(NULL, " \n");

        if (pid_string == NULL || timer_string == NULL)
        {
            printf("Usage: send_task <pid> <task_timer>\n");
            return;
        }

        pid_t pid = (pid_t)strtol(pid_string, NULL, 10);
        int seconds = (int)strtol(timer_string, NULL, 10);

        send_task(pid, seconds);
    }
    else if (strcmp(token, "get_status") == 0)
    {
        char *pid_string = strtok(NULL, " \n");

        if (pid_string == NULL)
        {
            printf("Usage: get_status <pid>\n");
            return;
        }

        pid_t pid = (pid_t)strtol(pid_string, NULL, 10);

        get_status(pid);
    }
    else if (strcmp(token, "get_drivers") == 0)
    {
        get_drivers();
    }
    else if (strcmp(token, "exit") == 0)
    {
        for (int i = 0; i < driver_count; i++)
        {
            send_message(drivers[i].fd, "EXIT\n");
            close(drivers[i].fd);
        }

        while (wait(NULL) > 0)
        {
        }

        exit(EXIT_SUCCESS);
    }
    else
    {
        printf("Unknown command\n");
        printf("Available commands:\n");
        printf("  create_driver\n");
        printf("  send_task <pid> <task_timer>\n");
        printf("  get_status <pid>\n");
        printf("  get_drivers\n");
        printf("  exit\n");
    }
}

int main(void)
{
    init_drivers();

    struct pollfd fds[MAX_DRIVERS + 1];

    while (1)
    {
        fds[0].fd = STDIN_FILENO;
        fds[0].events = POLLIN;

        for (int i = 0; i < driver_count; i++)
        {
            fds[i + 1].fd = drivers[i].fd;
            fds[i + 1].events = POLLIN;
            fds[i + 1].revents = 0;
        }

        int result = poll(fds, driver_count + 1, -1);

        if (result == -1)
        {
            if (errno == EINTR)
                continue;

            perror("poll");
            break;
        }

        if (fds[0].revents & POLLIN)
        {
            char command[BUFFER_SIZE];

            printf("> ");
            fflush(stdout);

            if (fgets(command, sizeof(command), stdin) == NULL)
            {
                break;
            }

            process_command(command);
        }

        for (int i = 0; i < driver_count; i++)
        {
            if (fds[i + 1].revents & POLLIN)
            {
                update_driver_status(i);
            }

            if (fds[i + 1].revents & (POLLHUP | POLLERR | POLLNVAL))
            {
                printf("Driver %d disconnected\n", drivers[i].pid);

                close(drivers[i].fd);
                drivers[i].fd = -1;
            }
        }
    }

    return 0;
}
