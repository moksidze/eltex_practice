#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 5

struct contact
{
    char name[100];
    char surname[100];
    char patronymic[100];
    int age;
    char work[100];
    char position[100];
    char phone[15];
    char email[100];
    char link[150];
    char messenger[50];
} ;

struct contact people[SIZE];
int count = 0;

void Menu();
void AddContact();
void EditContact();
void DeleteContact();
void WatchContact();

int main(void)
{   
    Menu();

    return 0;
}

void Menu()
{
    int x;
    
    system("clear");
    printf("*МЕНЮ*\n\n");
    printf("1.Добавить контакт\n");
    printf("2.Изменить контакт\n");
    printf("3.Удалить контакт\n");
    printf("4.Посмотреть список контактов\n");
    printf("\nВыберите пункт меню:");

    scanf("%d", &x);
    switch(x)
    {   
        case 1:
            system("clear");
            printf("*Добавить контакт*\n\n");
            AddContact();
            break;
        case 2:
            system("clear");
            printf("*Изменить контакт*\n\n");
            EditContact();
            break;
        case 3:
            system("clear");
            printf("*Удалить контакт*\n\n");
            DeleteContact();
            break;
        case 4:
            system("clear");
            printf("*Посмотреть список контактов*\n\n");
            WatchContact();
            break;
        default:
            printf("*Вы вышли из меню*\n\n");
            break;
    }
}

void AddContact()
{   
    char x;

    if(count == SIZE)
    {
        printf("Телефонная книга заполнена!\n");

        printf("\nДля возвращения в меню введите любой символ:");
        scanf(" %c", &x);
            Menu();
    }

    while (getchar() != '\n');

    do 
    {
        printf("Введите имя(обязательно для заполнения):\n");
        fgets(people[count].name, sizeof(people[count].name), stdin);
        people[count].name[strcspn(people[count].name, "\n")] = '\0';
    }
    while (strlen(people[count].name) == 0);

    do 
    {
        printf("Введите фамилию(обязательно для заполнения):\n");
        fgets(people[count].surname, sizeof(people[count].surname), stdin);
        people[count].surname[strcspn(people[count].surname, "\n")] = '\0';
    }
    while (strlen(people[count].surname) == 0);

    printf("Введите отчество:\n");
    fgets(people[count].patronymic, sizeof(people[count].patronymic), stdin);
    people[count].patronymic[strcspn(people[count].patronymic, "\n")] = '\0';

    printf("Введите возраст:\n");
    scanf("%d", &people[count].age);

    while (getchar() != '\n');
    printf("Введите место работы:\n");
    fgets(people[count].work, sizeof(people[count].work), stdin);
    people[count].work[strcspn(people[count].work, "\n")] = '\0';

    printf("Введите должность:\n");
    fgets(people[count].position, sizeof(people[count].position), stdin);
    people[count].position[strcspn(people[count].position, "\n")] = '\0';

    printf("Введите номер телефона:\n");
    fgets(people[count].phone, sizeof(people[count].phone), stdin);
    people[count].phone[strcspn(people[count].phone, "\n")] = '\0';

    printf("Введите адрес электронной почты:\n");
    fgets(people[count].email, sizeof(people[count].email), stdin);
    people[count].email[strcspn(people[count].email, "\n")] = '\0';

    printf("Введите ссылку в мессенджер:\n");
    fgets(people[count].link, sizeof(people[count].link), stdin);
    people[count].link[strcspn(people[count].link, "\n")] = '\0';

    printf("Введите мессенджер:\n");
    fgets(people[count].messenger, sizeof(people[count].messenger), stdin);
    people[count].messenger[strcspn(people[count].messenger, "\n")] = '\0';

    count++;

    Menu();
}

void EditContact()
{
    char surname[100];
    char name[100];
    int field;
    int found = -1;
    char x;

    if (count == 0)
    {
        printf("Телефонная книга пуста!\n");

        printf("\nДля возвращения в меню введите любой символ:");
        scanf(" %c", &x);
        Menu();
    }

    while (getchar() != '\n');
    
    printf("Введите имя контакта: ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = '\0';

    printf("Введите фамилию контакта: ");
    fgets(surname, sizeof(surname), stdin);
    surname[strcspn(surname, "\n")] = '\0';

    for (int i = 0; i < count; i++)
    {
        if (strcmp(people[i].surname, surname) == 0 && strcmp(people[i].name, name) == 0)
        {
            found = i;
            break;
        }
    }

    if (found == -1)
    {
        printf("Контакт не найден!\n");
        return;
    }

    printf("\nЧто изменить?\n");
    printf("1. Имя\n");
    printf("2. Фамилия\n");
    printf("3. Отчество\n");
    printf("4. Возраст\n");
    printf("5. Место работы\n");
    printf("6. Должность\n");
    printf("7. Номер телефона\n");
    printf("8. Адрес электронной почты\n");
    printf("9. Ссылку на мессенджер\n");
    printf("10. Мессенджер\n");

    scanf("%d", &field);
    while (getchar() != '\n');

    switch(field)
    {
        case 1:
            printf("Введите новое имя:\n");
            fgets(people[found].name, sizeof(people[found].name), stdin);
            people[found].name[strcspn(people[found].name, "\n")] = '\0';
            break;
        case 2:
            printf("Введите новую фамилию:\n");
            fgets(people[found].surname, sizeof(people[found].surname), stdin);
            people[found].surname[strcspn(people[found].surname, "\n")] = '\0';
            break;
        case 3:
            printf("Введите новое отчество:\n");
            fgets(people[found].patronymic, sizeof(people[found].patronymic), stdin);
            people[found].patronymic[strcspn(people[found].patronymic, "\n")] = '\0';
            break;
        case 4:
            printf("Введите новый возраст:\n");
            scanf("%d", &people[found].age);
            break;
        case 5:
            while(getchar() != '\n');
            printf("Введите новое место работы:\n");
            fgets(people[found].work, sizeof(people[found].work), stdin);
            people[found].work[strcspn(people[found].work, "\n")] = '\0';
            break;
        case 6:
            printf("Введите новую должность:\n");
            fgets(people[found].position, sizeof(people[found].position), stdin);
            people[found].position[strcspn(people[found].position, "\n")] = '\0';
            break;
        case 7:
            printf("Введите новый номер телефона:\n");
            fgets(people[found].phone, sizeof(people[found].phone), stdin);
            people[found].phone[strcspn(people[found].phone, "\n")] = '\0';
            break;
        case 8:
            printf("Введите новый адрес электронной почты:\n");
            fgets(people[found].email, sizeof(people[found].email), stdin);
            people[found].email[strcspn(people[found].email, "\n")] = '\0';
            break;
        case 9:
            printf("Введите новую ссылку на профиль:\n");
            fgets(people[found].link, sizeof(people[found].link), stdin);
            people[found].link[strcspn(people[found].link, "\n")] = '\0';
            break;
        case 10:
            printf("Введите новый мессенджер:\n");
            fgets(people[found].messenger, sizeof(people[found].messenger), stdin);
            people[found].messenger[strcspn(people[found].messenger, "\n")] = '\0';
            break;
        default:
            printf("Такого пункта нет!\n");
    }

    printf("Контакт успешно изменен.\n");
    
    printf("\nДля возвращения в меню введите любой символ:");
    scanf("%c", &x);
    Menu();
}

void DeleteContact()
{
    char name[100];
    char surname[100];
    char x;
    int found = -1;

    if (count == 0)
    {
        printf("Телефонная книга пуста!\n");

        printf("\nДля возвращения в меню введите любой символ: ");
        scanf(" %c", &x);
        Menu();
        return;
    }

    while (getchar() != '\n');

    printf("Введите имя контакта: ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = '\0';

    printf("Введите фамилию контакта: ");
    fgets(surname, sizeof(surname), stdin);
    surname[strcspn(surname, "\n")] = '\0';

    for (int i = 0; i < count; i++)
    {
        if (strcmp(people[i].name, name) == 0 && strcmp(people[i].surname, surname) == 0)
        {
            found = i;
            break;
        }
    }

    if (found == -1)
    {
        printf("Контакт не найден!\n");

        printf("\nДля возвращения в меню введите любой символ: ");
        scanf(" %c", &x);
        Menu();
        return;
    }

    for (int i = found; i < count - 1; i++)
    {
        people[i] = people[i + 1];
    }

    count--;

    printf("Контакт успешно удален!\n");

    printf("\nДля возвращения в меню введите любой символ: ");
    scanf(" %c", &x);
    Menu();
    
}

void WatchContact()
{   
    char x;

    if(count == 0)
    {
        printf("Телефонная книга пуста!\n");

        printf("\nДля возвращения в меню введите любой символ:");
        scanf(" %c", &x);
        Menu();
    }

    for(int i = 0; i < count; i++)
    {
        printf("Имя: %s ", people[i].name);
        printf("Фамилия: %s ", people[i].surname);
        printf("Отчество: %s ", people[i].patronymic);
        printf("Возраст: %d\n", people[i].age);
        printf("Место работы: %s ", people[i].work);
        printf("Должность: %s\n", people[i].position);
        printf("Номер телефона: %s\n", people[i].phone);
        printf("Адрес электронной почты: %s\n", people[i].email);
        printf("Ссылка на мессенджер: %s ", people[i].link);
        printf("Мессенджер: %s\n", people[i].messenger);
    }

    printf("\nДля возвращения в меню введите любой символ:");
    scanf(" %c", &x);
    Menu();
}