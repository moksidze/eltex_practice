#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/stat.h>

static int sockfd = -1;

static char username[USERNAME_SIZE];

static void signal_handler(int signal)
{
    (void)signal;

    if (sockfd != -1)
    {
        close(sockfd);
    }

    printf("\nВы отключились от сервера.\n");

    exit(EXIT_SUCCESS);
}

static int send_join(void)
{
    MessageHeader header;

    memset(&header, 0, sizeof(header));

    header.type = MSG_JOIN;

    header.size = 0;

    strncpy(header.username, username, USERNAME_SIZE - 1);

    header.username[USERNAME_SIZE - 1] = '\0';

    return send_all(sockfd, &header, sizeof(header));
}

static int send_chat_message(const char *text)
{
    MessageHeader header;

    memset(&header, 0, sizeof(header));

    header.type = MSG_CHAT;

    header.size = strlen(text);

    strncpy(header.username, username, USERNAME_SIZE - 1);

    header.username[USERNAME_SIZE - 1] = '\0';

    if (send_all(sockfd, &header, sizeof(header)) == -1)
    {
        return -1;
    }

    if (header.size > 0)
    {
        if (send_all(sockfd, text, header.size) == -1)
        {
            return -1;
        }
    }


    return 0;
}

static const char *get_filename(const char *path)
{
    const char *slash = strrchr(path, '/');

    if (slash != NULL)
    {
        return slash + 1;
    }

    return path;
}

static int send_file(const char *path)
{
    FILE *file = fopen(path, "rb");

    if (file == NULL)
    {
        perror("fopen");

        return -1;
    }

    if (fseek(file, 0, SEEK_END) != 0)
    {
        perror("fseek");

        fclose(file);

        return -1;
    }

    long file_size = ftell(file);

    if (file_size < 0)
    {
        perror("ftell");

        fclose(file);

        return -1;
    }

    rewind(file);

    if ((unsigned long)file_size > UINT32_MAX)
    {
        fprintf(stderr, "Файл слишком большой.\n");

        fclose(file);

        return -1;
    }


    const char *filename = get_filename(path);

    MessageHeader header;

    memset(&header, 0, sizeof(header));

    header.type = MSG_FILE;

    header.size = (uint32_t)file_size;

    strncpy(header.username, username, USERNAME_SIZE - 1);

    header.username[USERNAME_SIZE - 1] = '\0';

    strncpy(header.filename, filename, FILENAME_SIZE - 1);

    header.filename[FILENAME_SIZE - 1] = '\0';

    printf("Отправка файла \"%s\" (%ld байт)...\n", filename, file_size);

    if (send_all(sockfd, &header, sizeof(header)) == -1)
    {
        perror("send header");

        fclose(file);

        return -1;
    }

    char buffer[BUFFER_SIZE];

    long remaining = file_size;

    while (remaining > 0)
    {
        size_t to_read = sizeof(buffer);

        if (remaining < (long)to_read)
        {
            to_read = (size_t)remaining;
        }

        size_t bytes_read = fread(buffer, 1, to_read, file);

        if (bytes_read == 0)
        {
            if (ferror(file))
            {
                perror("fread");
            }

            fclose(file);

            return -1;
        }


        if (send_all(sockfd, buffer, bytes_read) == -1)
        {
            perror("send file");

            fclose(file);

            return -1;
        }

        remaining -= bytes_read;
    }

    fclose(file);

    printf("Файл \"%s\" успешно отправлен серверу.\n", filename);

    return 0;
}

static void make_received_filename(char *result, size_t result_size, const char *filename)
{
    snprintf(result, result_size, "received_%s", get_filename(filename));
}

static int receive_file(const MessageHeader *header)
{
    char path[512];

    make_received_filename(path, sizeof(path), header->filename);

    FILE *file = fopen(path, "wb");

    if (file == NULL)
    {
        perror("fopen");

        unsigned char buffer[BUFFER_SIZE];

        uint32_t remaining = header->size;

        while (remaining > 0)
        {
            size_t chunk = sizeof(buffer);

            if (remaining < chunk)
            {
                chunk = remaining;
            }

            if (recv_all(sockfd, buffer, chunk) == -1)
            {
                return -1;
            }

            remaining -= chunk;
        }

        return -1;
    }


    printf("\nПолучение файла \"%s\" от %s (%u байт)...\n", header->filename, header->username, header->size);

    unsigned char buffer[BUFFER_SIZE];

    uint32_t remaining = header->size;

    while (remaining > 0)
    {
        size_t chunk = sizeof(buffer);

        if (remaining < chunk)
        {
            chunk = remaining;
        }

        if (recv_all(sockfd, buffer, chunk) == -1)
        {
            fclose(file);

            return -1;
        }

        if (fwrite(buffer, 1, chunk, file) != chunk)
        {
            perror("fwrite");

            fclose(file);

            return -1;
        }

        remaining -= chunk;
    }

    fclose(file);

    printf("Файл сохранён как: %s\n", path);

    return 0;
}

static int receive_message(void)
{
    MessageHeader header;

    if (recv_all(sockfd, &header, sizeof(header)) == -1)
    {
        return -1;
    }

    if (header.type == MSG_CHAT)
    {

        if (header.size > BUFFER_SIZE)
        {
            fprintf(stderr, "Получено слишком большое сообщение.\n");

            return -1;
        }

        char buffer[BUFFER_SIZE + 1];

        if (header.size > 0)
        {
            if (recv_all(sockfd, buffer, header.size) == -1)
            {
                return -1;
            }
        }

        buffer[header.size] = '\0';

        printf("\n[%s]: %s\n", header.username, buffer);

        printf("> ");
        fflush(stdout);
    }

    else if (header.type == MSG_JOIN)
    {
        printf("\n[SYSTEM] %s подключился к чату.\n",header.username);

        printf("> ");
        fflush(stdout);
    }

    else if (header.type == MSG_LEAVE)
    {
        printf("\n[SYSTEM] %s отключился от чата.\n",header.username);

        printf("> ");
        fflush(stdout);
    }

    else if (header.type == MSG_FILE)
    {
        if (receive_file(&header) == -1)
        {
            return -1;
        }

        printf("> ");
        fflush(stdout);
    }
    else
    {
        fprintf(stderr, "Неизвестный тип сообщения: %u\n", header.type);

        return -1;
    }

    return 0;
}

static int connect_to_server(const char *server_ip)
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);

    if (fd == -1)
    {
        perror("socket");

        return -1;
    }

    struct sockaddr_in server_address;

    memset(&server_address, 0, sizeof(server_address));

    server_address.sin_family = AF_INET;

    server_address.sin_port = htons(SERVER_PORT);

    if (inet_pton(AF_INET, server_ip, &server_address.sin_addr) <= 0)
    {
        fprintf(stderr, "Некорректный IP-адрес сервера: %s\n", server_ip);

        close(fd);

        return -1;
    }


    if (connect(fd, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
    {
        perror("connect");

        close(fd);

        return -1;
    }

    return fd;
}

int main(int argc, char *argv[])
{

    if (argc != 2)
    {
        printf("Использование:\n""  %s <IP-сервера>\n\n""Пример:\n""  %s 127.0.0.1\n", argv[0], argv[0]);

        return EXIT_FAILURE;
    }

    printf("Введите ваше имя: ");

    if (fgets(username, sizeof(username),stdin) == NULL)
    {
        return EXIT_FAILURE;
    }

    username[strcspn(username, "\n")] = '\0';

    if (strlen(username) == 0)
    {
        strcpy(username, "Anonymous");
    }

    sockfd = connect_to_server(argv[1]);

    if (sockfd == -1)
    {
        return EXIT_FAILURE;
    }

    printf("Подключение к серверу %s:%d успешно.\n",argv[1],SERVER_PORT);

    struct sigaction sa;

    memset(&sa, 0, sizeof(sa));

    sa.sa_handler = signal_handler;

    sigemptyset(&sa.sa_mask);

    sa.sa_flags = 0;

    if (sigaction(SIGINT, &sa, NULL) == -1)
    {
        perror("sigaction");

        close(sockfd);

        return EXIT_FAILURE;
    }

    if (send_join() == -1)
    {
        perror("send join");

        close(sockfd);

        return EXIT_FAILURE;
    }

    printf("\n");
    printf("Вы вошли в чат.\n");
    printf("\n");
    printf("Команды:\n");
    printf("  /file <путь>  - отправить файл\n");
    printf("  /quit         - выйти из чата\n");
    printf("\n");

    printf("> ");
    fflush(stdout);

    while (1)
    {
        fd_set readfds;

        FD_ZERO(&readfds);

        FD_SET(STDIN_FILENO,&readfds);

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
            char input[BUFFER_SIZE];

            if (fgets(input, sizeof(input), stdin) == NULL)
            {
                break;
            }

            input[strcspn(input, "\n")] = '\0';

            if (strlen(input) == 0)
            {
                printf("> ");
                fflush(stdout);

                continue;
            }

            if (strcmp(input, "/quit") == 0)
            {
                break;
            }

            if (strncmp(input,"/file ", 6) == 0)
            {
                const char *path = input + 6;

                if (strlen(path) == 0)
                {
                    printf("Укажите путь к файлу.\n");
                }
                else
                {
                    send_file(path);
                }

                printf("> ");
                fflush(stdout);

                continue;
            }

            if (send_chat_message(input) == -1)
            {
                fprintf(stderr, "Ошибка отправки сообщения.\n");

                break;
            }

            printf("> ");
            fflush(stdout);
        }

        if (FD_ISSET(sockfd, &readfds))
        {
            if (receive_message() == -1)
            {
                printf("\nСоединение с сервером закрыто.\n");

                break;
            }
        }
    }

    close(sockfd);

    sockfd = -1;

    printf("\nКлиент завершил работу.\n");

    return EXIT_SUCCESS;
}