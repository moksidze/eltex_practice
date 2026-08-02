#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "book.h"

void Menu(void);

int main(void)
{
    Menu();

    return 0;
}

void PrintContacts(contact *root)
{
    if (root == NULL)
        return;

    PrintContacts(root->left);

    printf("\nИмя: %s\n", root->name);
    printf("Фамилия: %s\n", root->surname);
    printf("Отчество: %s\n", root->patronymic);
    printf("Возраст: %d\n", root->age);
    printf("Место работы: %s\n", root->work);
    printf("Должность: %s\n", root->position);
    printf("Телефон: %s\n", root->phone);
    printf("Email: %s\n", root->email);
    printf("Ссылка: %s\n", root->link);
    printf("Мессенджер: %s\n", root->messenger);

    PrintContacts(root->right);
}

void Menu(void)
{
    int choice;

    while (1)
    {
        contact newContact;

        char name[100];
        char surname[100];
        char value[100];

        int field;
        int age = 0;

        system("clear");

        printf("*МЕНЮ*\n\n");
        printf("1. Добавить контакт\n");
        printf("2. Изменить контакт\n");
        printf("3. Удалить контакт\n");
        printf("4. Посмотреть список контактов\n");
        printf("0. Выход\n\n");

        printf("Выберите пункт меню: ");
        scanf("%d", &choice);
        while (getchar() != '\n');

        switch (choice)
        {
        case 1:

            printf("\nВведите имя: ");
            fgets(newContact.name, sizeof(newContact.name), stdin);
            newContact.name[strcspn(newContact.name, "\n")] = '\0';

            printf("Введите фамилию: ");
            fgets(newContact.surname, sizeof(newContact.surname), stdin);
            newContact.surname[strcspn(newContact.surname, "\n")] = '\0';

            printf("Введите отчество: ");
            fgets(newContact.patronymic, sizeof(newContact.patronymic), stdin);
            newContact.patronymic[strcspn(newContact.patronymic, "\n")] = '\0';

            printf("Введите возраст: ");
            scanf("%d", &newContact.age);
            while (getchar() != '\n');

            printf("Введите место работы: ");
            fgets(newContact.work, sizeof(newContact.work), stdin);
            newContact.work[strcspn(newContact.work, "\n")] = '\0';

            printf("Введите должность: ");
            fgets(newContact.position, sizeof(newContact.position), stdin);
            newContact.position[strcspn(newContact.position, "\n")] = '\0';

            printf("Введите телефон: ");
            fgets(newContact.phone, sizeof(newContact.phone), stdin);
            newContact.phone[strcspn(newContact.phone, "\n")] = '\0';

            printf("Введите email: ");
            fgets(newContact.email, sizeof(newContact.email), stdin);
            newContact.email[strcspn(newContact.email, "\n")] = '\0';

            printf("Введите ссылку: ");
            fgets(newContact.link, sizeof(newContact.link), stdin);
            newContact.link[strcspn(newContact.link, "\n")] = '\0';

            printf("Введите мессенджер: ");
            fgets(newContact.messenger, sizeof(newContact.messenger), stdin);
            newContact.messenger[strcspn(newContact.messenger, "\n")] = '\0';

            if (AddContact(&newContact))
                printf("\nКонтакт успешно добавлен.\n");
            else
                printf("\nОшибка добавления контакта.\n");

            break;

        case 2:

            printf("Введите имя контакта: ");
            fgets(name, sizeof(name), stdin);
            name[strcspn(name, "\n")] = '\0';

            printf("Введите фамилию контакта: ");
            fgets(surname, sizeof(surname), stdin);
            surname[strcspn(surname, "\n")] = '\0';

            printf("\nЧто изменить?\n");
            printf("1. Имя\n");
            printf("2. Фамилию\n");
            printf("3. Отчество\n");
            printf("4. Возраст\n");
            printf("5. Место работы\n");
            printf("6. Должность\n");
            printf("7. Телефон\n");
            printf("8. Email\n");
            printf("9. Ссылку\n");
            printf("10. Мессенджер\n");

            scanf("%d", &field);
            while (getchar() != '\n');

            if (field == 4)
            {
                printf("Введите новый возраст: ");
                scanf("%d", &age);
                while (getchar() != '\n');

                if (EditContact(name, surname, field, "", age))
                    printf("Контакт изменен.\n");
                else
                    printf("Контакт не найден.\n");
            }
            else
            {
                printf("Введите новое значение: ");
                fgets(value, sizeof(value), stdin);
                value[strcspn(value, "\n")] = '\0';

                if (EditContact(name, surname, field, value, 0))
                    printf("Контакт изменен.\n");
                else
                    printf("Контакт не найден.\n");
            }

            break;

        case 3:

            printf("Введите имя: ");
            fgets(name, sizeof(name), stdin);
            name[strcspn(name, "\n")] = '\0';

            printf("Введите фамилию: ");
            fgets(surname, sizeof(surname), stdin);
            surname[strcspn(surname, "\n")] = '\0';

            if (DeleteContact(name, surname))
                printf("Контакт удален.\n");
            else
                printf("Контакт не найден.\n");

            break;

       case 4:

            if (WatchContact() == NULL)
            {
                printf("Телефонная книга пуста.\n");
            }
            else
            {
                PrintContacts(WatchContact());
            }
            
            break;

        case 0:
            return;

        default:
            printf("Неверный пункт меню.\n");
        }

        printf("\nНажмите Enter...");
        getchar();
    }
}