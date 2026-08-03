#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Использование: %s файл1 файл2 ...\n", argv[0]);
        return 1;
    }

    char buffer[1024];

    for (int i = 1; i < argc; i++)
    {
        FILE *src = fopen(argv[i], "rb");
        if (src == NULL)
        {
            printf("Не удалось открыть %s\n", argv[i]);
            continue;
        }

        char newName[256];
        sprintf(newName, "copy_%s", argv[i]);

        FILE *dst = fopen(newName, "wb");
        if (dst == NULL)
        {
            printf("Не удалось создать %s\n", newName);
            fclose(src);
            continue;
        }

        size_t bytes;
        while ((bytes = fread(buffer, 1, sizeof(buffer), src)) > 0)
        {
            fwrite(buffer, 1, bytes, dst);
        }

        fclose(src);
        fclose(dst);

        printf("Файл %s скопирован в %s\n", argv[i], newName);
    }

    return 0;
}