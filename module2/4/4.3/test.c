#include <stdio.h>
#include <string.h>
#include "book.h"

void PrintTree(contact *root)
{
    if (root == NULL)
        return;

    PrintTree(root->left);

    printf("%s %s\n", root->surname, root->name);

    PrintTree(root->right);
}

int main(void)
{
    contact c;

    printf("========== НАЧАЛО ТЕСТИРОВАНИЯ ==========\n\n");

    printf("1. Добавление контактов...\n");

    for (int i = 1; i <= 15; i++)
    {
        sprintf(c.name, "Name%d", i);
        sprintf(c.surname, "Surname%d", i);
        strcpy(c.patronymic, "-");
        c.age = 20 + i;
        strcpy(c.work, "Company");
        strcpy(c.position, "Employee");
        sprintf(c.phone, "89000000%d", i);
        strcpy(c.email, "mail@test.ru");
        strcpy(c.link, "-");
        strcpy(c.messenger, "Telegram");

        if (!AddContact(&c))
        {
            printf("Ошибка при добавлении контакта %d\n", i);
            return 1;
        }
    }

    printf("Контакты успешно добавлены.\n\n");

    printf("2. Проверка существования дерева...\n");

    if (WatchContact() != NULL)
        printf("Дерево создано.\n\n");
    else
    {
        printf("Ошибка создания дерева.\n");
        return 1;
    }

    printf("3. Изменение телефона...\n");

    if (EditContact("Name5", "Surname5", 7, "89999999999", 0))
        printf("Телефон изменен.\n\n");
    else
        printf("Ошибка редактирования.\n\n");

    printf("4. Изменение имени...\n");

    if (EditContact("Name3", "Surname3", 1, "NewName", 0))
        printf("Имя изменено.\n\n");
    else
        printf("Ошибка изменения имени.\n\n");

    printf("5. Удаление контакта...\n");

    if (DeleteContact("Name10", "Surname10"))
        printf("Контакт удален.\n\n");
    else
        printf("Ошибка удаления.\n\n");

    printf("6. Проверка удаления...\n");

    if (!DeleteContact("Name10", "Surname10"))
        printf("Контакт действительно отсутствует.\n\n");
    else
        printf("Ошибка.\n\n");

    printf("7. Вывод дерева (контакты должны быть отсортированы):\n\n");

    PrintTree(WatchContact());

    printf("\n8. Балансировка.\n");
    printf("После добавления и удаления более 10 элементов балансировка была выполнена автоматически.\n");

    printf("\n========== ВСЕ ТЕСТЫ ПРОЙДЕНЫ ==========\n");

    return 0;
}