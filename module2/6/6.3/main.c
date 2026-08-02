#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <dlfcn.h>
#include <string.h>

#define MAX_LIBS 20

typedef double (*Operation)(double, double);
typedef const char *(*GetName)(void);

struct command
{
    char name[50];
    Operation funk;
    void *handle;
};

int main(void)
{
    double a, b;

    printf("Введите два числа: ");
    scanf("%lf %lf", &a, &b);

    while (1)
    {
        struct command menu[MAX_LIBS];
        int count = 0;

        DIR *dir = opendir("./libs");

        if (dir == NULL)
        {
            perror("Не удалось открыть каталог libs");
            return 1;
        }

        struct dirent *entry;

        while ((entry = readdir(dir)) != NULL)
        {
            if (strstr(entry->d_name, ".so") == NULL)
                continue;

            char path[512];
            snprintf(path, sizeof(path), "./libs/%s", entry->d_name);

            void *handle = dlopen(path, RTLD_LAZY);

            if (handle == NULL)
                continue;

            Operation op = (Operation)dlsym(handle, "Operation");
            GetName getName = (GetName)dlsym(handle, "GetName");

            if (op == NULL || getName == NULL)
            {
                dlclose(handle);
                continue;
            }

            strcpy(menu[count].name, getName());
            menu[count].funk = op;
            menu[count].handle = handle;

            count++;
        }

        closedir(dir);

        system("clear");

        printf("Числа: %.2lf %.2lf\n\n", a, b);

        if (count == 0)
        {
            printf("Библиотеки не найдены.\n");
        }
        else
        {
            printf("Доступные операции:\n");

            for (int i = 0; i < count; i++)
            {
                printf("%d. %s\n", i + 1, menu[i].name);
            }
        }

        printf("0. Выход\n");

        int x;

        printf("\nВыберите пункт: ");
        scanf("%d", &x);

        if (x == 0)
        {
            for (int i = 0; i < count; i++)
                dlclose(menu[i].handle);

            break;
        }

        if (x >= 1 && x <= count)
        {
            printf("\nРезультат: %.2lf\n", menu[x - 1].funk(a, b));
        }
        else
        {
            printf("\nНеверный выбор.\n");
        }

        printf("\nНажмите Enter для продолжения...");
        getchar();
        getchar();

        for (int i = 0; i < count; i++)
            dlclose(menu[i].handle);
    }

    return 0;
}