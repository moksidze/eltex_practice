#include "chat.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <sys/select.h>

static int sockfd = -1;
static char username[64];

void signal_handler(int signal)
{
    (void)signal;

    char message[BUFFER_SIZE];

    snprintf(message, sizeof(message), "[SYSTEM] %s отключился от чата", username);

    send_broadcast(sockfd, message);

    close_socket(sockfd);

    printf("\nВы отключились от чата.\n");

    exit(EXIT_SUCCESS);
}


int main(void)
{
    char buffer[BUFFER_SIZE];
    char message[BUFFER_SIZE];

    printf("Введите ваше имя: ");

    if (fgets(username, sizeof(username), stdin) == NULL)
    {
        return EXIT_FAILURE;
    }

    username[strcspn(username, "\n")] = '\0';

    if (strlen(username) == 0)
    {
        strcpy(username, "Anonymous");
    }

    sockfd = create_socket();

    if (sockfd == -1)
    {
        return EXIT_FAILURE;
    }

    struct sigaction sa;

    memset(&sa, 0, sizeof(sa));

    sa.sa_handler = signal_handler;

    sigemptyset(&sa.sa_mask);

    sa.sa_flags = 0;

    if (sigaction(SIGINT, &sa, NULL) == -1)
    {
        perror("sigaction");
        close_socket(sockfd);
        return EXIT_FAILURE;
    }

    snprintf(message, sizeof(message), "[SYSTEM] %s подключился к чату", username);

    send_broadcast(sockfd, message);

    printf("\nВы вошли в чат.\n");
    printf("Для выхода нажмите Ctrl+C.\n\n");

    printf("> ");
    fflush(stdout);

    while (1)
    {
        fd_set readfds;

        FD_ZERO(&readfds);

        FD_SET(STDIN_FILENO, &readfds);
        FD_SET(sockfd, &readfds);

        int max_fd = sockfd;

        if (STDIN_FILENO > max_fd)
        {
            max_fd = STDIN_FILENO;
        }

        int result = select(max_fd + 1, &readfds, NULL, NULL, NULL);

        if (result == -1)
        {
            if (errno == EINTR)
            {
                continue;
            }

            perror("select");
            break;
        }

        if (FD_ISSET(STDIN_FILENO, &readfds))
        {
            if (fgets(buffer, sizeof(buffer), stdin) == NULL)
            {
                break;
            }

            buffer[strcspn(buffer, "\n")] = '\0';

            if (strlen(buffer) == 0)
            {
                printf("> ");
                fflush(stdout);
                continue;
            }

            snprintf(message, sizeof(message), "[%.*s] %.*s", 60, username, 950, buffer);

            if (send_message(sockfd, message) == -1)
            {
                fprintf(stderr, "Не удалось отправить сообщение\n");
            }

            printf("> ");
            fflush(stdout);
        }

        if (FD_ISSET(sockfd, &readfds))
        {
            int bytes = receive_message(sockfd, buffer, sizeof(buffer));

            if (bytes > 0)
            {
                printf("\r%s\n", buffer);
                printf("> ");
                fflush(stdout);
            }
        }
    }

    close_socket(sockfd);

    return EXIT_SUCCESS;
}