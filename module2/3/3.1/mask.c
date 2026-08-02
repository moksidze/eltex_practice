#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include "mask.h"

mode_t currentMask = 0;

void Converter()
{   
    char rights[10];
    char x;

    printf("\nВведите права доступа буквами или цифрами:");
    scanf("%s", rights);

    int length = strlen(rights);
    
    currentMask = 0;

    if (length == 3)
    {
        printf("Результат:");

        for(int i = 0; i < length; i++)
        {
            int digit = rights[i] - '0';

            currentMask <<= 3;
            currentMask |= digit;

            printf("%d", (digit >> 2) & 1);
            printf("%d", (digit >> 1) & 1);
            printf("%d", digit & 1);
        }
    }
    else if (length == 9)
    {
        for(int i = 0; i < length; i++)
        {   
            if(rights[i] == '-')
                printf("%s", "0");
            else
                printf("%s", "1");
        }
    }
    else
    {
        printf("Ошибка!\n");
    }

    printf("\n");
    printf("\nДля возвращения в меню введите любой символ:");
    scanf(" %c", &x);
    Menu();
}

void FileRights()
{
    char filename[256];
    struct stat info;
    char x;

    printf("Введите имя файла: ");
    scanf("%s", filename);

    if (stat(filename, &info) != 0)
    {
        printf("Ошибка! Файл не найден.\n");
        return;
    }

    printf("\nФайл: %s\n", filename);

    printf("\nБуквенное представление: ");

    printf((info.st_mode & S_IRUSR) ? "r" : "-");
    printf((info.st_mode & S_IWUSR) ? "w" : "-");
    printf((info.st_mode & S_IXUSR) ? "x" : "-");

    printf((info.st_mode & S_IRGRP) ? "r" : "-");
    printf((info.st_mode & S_IWGRP) ? "w" : "-");
    printf((info.st_mode & S_IXGRP) ? "x" : "-");

    printf((info.st_mode & S_IROTH) ? "r" : "-");
    printf((info.st_mode & S_IWOTH) ? "w" : "-");
    printf((info.st_mode & S_IXOTH) ? "x" : "-");

    printf("\nБитовое представление: ");

    printf("%d", (info.st_mode & S_IRUSR) ? 1 : 0);
    printf("%d", (info.st_mode & S_IWUSR) ? 1 : 0);
    printf("%d", (info.st_mode & S_IXUSR) ? 1 : 0);

    printf("%d", (info.st_mode & S_IRGRP) ? 1 : 0);
    printf("%d", (info.st_mode & S_IWGRP) ? 1 : 0);
    printf("%d", (info.st_mode & S_IXGRP) ? 1 : 0);

    printf("%d", (info.st_mode & S_IROTH) ? 1 : 0);
    printf("%d", (info.st_mode & S_IWOTH) ? 1 : 0);
    printf("%d", (info.st_mode & S_IXOTH) ? 1 : 0);

    int owner = 0;
    int group = 0;
    int other = 0;

    if (info.st_mode & S_IRUSR) owner += 4;
    if (info.st_mode & S_IWUSR) owner += 2;
    if (info.st_mode & S_IXUSR) owner += 1;

    if (info.st_mode & S_IRGRP) group += 4;
    if (info.st_mode & S_IWGRP) group += 2;
    if (info.st_mode & S_IXGRP) group += 1;

    if (info.st_mode & S_IROTH) other += 4;
    if (info.st_mode & S_IWOTH) other += 2;
    if (info.st_mode & S_IXOTH) other += 1;

    printf("\nЦифровое представление: %d%d%d\n", owner, group, other);

    printf("\nДля возвращения в меню введите любой символ:");
    scanf(" %c", &x);
    Menu();
}

void ChangeRights()
{
    char cmd[20];

    printf("Бинарная маска: ");

    for (int i = 8; i >= 0; i--)
        printf("%d", (currentMask >> i) & 1);

    printf("\n");

    printf("Введите команду (например u+x, go-wx, u=rw, a+rwx): ");
    scanf("%s", cmd);

    char *op = strpbrk(cmd, "+-=");

    if (op == NULL)
    {
        printf("Ошибка! Неверная команда.\n");
        return;
    }

    char action = *op;

    if (action == '=')
    {
        for (char *w = cmd; w < op; w++)
        {
            switch (*w)
            {
                case 'u':
                    currentMask &= ~(S_IRUSR | S_IWUSR | S_IXUSR);
                    break;

                case 'g':
                    currentMask &= ~(S_IRGRP | S_IWGRP | S_IXGRP);
                    break;

                case 'o':
                    currentMask &= ~(S_IROTH | S_IWOTH | S_IXOTH);
                    break;

                case 'a':
                    currentMask &= ~(S_IRUSR | S_IWUSR | S_IXUSR |
                                     S_IRGRP | S_IWGRP | S_IXGRP |
                                     S_IROTH | S_IWOTH | S_IXOTH);
                    break;
            }
        }
    }

    for (char *w = cmd; w < op; w++)
    {
        for (char *p = op + 1; *p != '\0'; p++)
        {
            mode_t bit = 0;

            switch (*w)
            {
                case 'u':
                    if (*p == 'r') bit = S_IRUSR;
                    else if (*p == 'w') bit = S_IWUSR;
                    else if (*p == 'x') bit = S_IXUSR;
                    break;

                case 'g':
                    if (*p == 'r') bit = S_IRGRP;
                    else if (*p == 'w') bit = S_IWGRP;
                    else if (*p == 'x') bit = S_IXGRP;
                    break;

                case 'o':
                    if (*p == 'r') bit = S_IROTH;
                    else if (*p == 'w') bit = S_IWOTH;
                    else if (*p == 'x') bit = S_IXOTH;
                    break;

                case 'a':
                    if (*p == 'r')
                        bit = S_IRUSR | S_IRGRP | S_IROTH;
                    else if (*p == 'w')
                        bit = S_IWUSR | S_IWGRP | S_IWOTH;
                    else if (*p == 'x')
                        bit = S_IXUSR | S_IXGRP | S_IXOTH;
                    break;
            }

            if (bit == 0)
                continue;

            if (action == '+')
                currentMask |= bit;
            else if (action == '-')
                currentMask &= ~bit;
            else if (action == '=')
                currentMask |= bit;
        }
    }

    printf("\nНовые права:\n");

    printf("Буквенное: ");

    printf((currentMask & S_IRUSR) ? "r" : "-");
    printf((currentMask & S_IWUSR) ? "w" : "-");
    printf((currentMask & S_IXUSR) ? "x" : "-");

    printf((currentMask & S_IRGRP) ? "r" : "-");
    printf((currentMask & S_IWGRP) ? "w" : "-");
    printf((currentMask & S_IXGRP) ? "x" : "-");

    printf((currentMask & S_IROTH) ? "r" : "-");
    printf((currentMask & S_IWOTH) ? "w" : "-");
    printf((currentMask & S_IXOTH) ? "x" : "-");

    printf("\nБитовое: ");

    printf("%d", (currentMask & S_IRUSR) ? 1 : 0);
    printf("%d", (currentMask & S_IWUSR) ? 1 : 0);
    printf("%d", (currentMask & S_IXUSR) ? 1 : 0);

    printf("%d", (currentMask & S_IRGRP) ? 1 : 0);
    printf("%d", (currentMask & S_IWGRP) ? 1 : 0);
    printf("%d", (currentMask & S_IXGRP) ? 1 : 0);

    printf("%d", (currentMask & S_IROTH) ? 1 : 0);
    printf("%d", (currentMask & S_IWOTH) ? 1 : 0);
    printf("%d", (currentMask & S_IXOTH) ? 1 : 0);

    int owner = 0, group = 0, other = 0;

    if (currentMask & S_IRUSR) owner += 4;
    if (currentMask & S_IWUSR) owner += 2;
    if (currentMask & S_IXUSR) owner += 1;

    if (currentMask & S_IRGRP) group += 4;
    if (currentMask & S_IWGRP) group += 2;
    if (currentMask & S_IXGRP) group += 1;

    if (currentMask & S_IROTH) other += 4;
    if (currentMask & S_IWOTH) other += 2;
    if (currentMask & S_IXOTH) other += 1;

    printf("\nЦифровое: %d%d%d\n", owner, group, other);
}