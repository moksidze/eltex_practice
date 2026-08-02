#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "book.h"

int main(void)
{
    contact c1 = {0};
    strcpy(c1.name, "Иван");
    strcpy(c1.surname, "Петров");
    c1.age = 20;

    contact c2 = {0};
    strcpy(c2.name, "Анна");
    strcpy(c2.surname, "Иванова");
    c2.age = 19;

    contact c3 = {0};
    strcpy(c3.name, "Сергей");
    strcpy(c3.surname, "Сидоров");
    c3.age = 25;

    printf("=== Тест AddContact ===\n");

    assert(AddContact(&c1) == 1);
    assert(AddContact(&c2) == 1);
    assert(AddContact(&c3) == 1);

    contact *p = WatchContact();

    /* Проверка сортировки */
    assert(strcmp(p->surname, "Иванова") == 0);
    p = p->next;
    assert(strcmp(p->surname, "Петров") == 0);
    p = p->next;
    assert(strcmp(p->surname, "Сидоров") == 0);

    printf("AddContact: OK\n");

    printf("=== Тест EditContact ===\n");

    assert(EditContact("Иван", "Петров", 7, "89991234567", 0) == 1);

    p = WatchContact();
    while (p != NULL)
    {
        if (strcmp(p->name, "Иван") == 0 &&
            strcmp(p->surname, "Петров") == 0)
        {
            break;
        }

        p = p->next;
    }

    assert(p != NULL);
    assert(strcmp(p->phone, "89991234567") == 0);

    printf("EditContact: OK\n");

    printf("=== Тест DeleteContact ===\n");

    assert(DeleteContact("Иван", "Петров") == 1);

    p = WatchContact();

    while (p != NULL)
    {
        assert(!(strcmp(p->name, "Иван") == 0 &&
                 strcmp(p->surname, "Петров") == 0));

        p = p->next;
    }

    printf("DeleteContact: OK\n");

    printf("=== Тест удаления несуществующего контакта ===\n");

    assert(DeleteContact("Не", "Существует") == 0);

    printf("DeleteContact (несуществующий): OK\n");

    printf("\nВсе тесты успешно пройдены!\n");

    return 0;
}