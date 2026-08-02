#ifndef BOOK_H
#define BOOK_H

#define SIZE 100

typedef struct contact
{
    char name[SIZE];
    char surname[SIZE];
    char patronymic[SIZE];
    int age;
    char work[SIZE];
    char position[SIZE];
    char phone[SIZE];
    char email[SIZE];
    char link[SIZE];
    char messenger[SIZE];

    struct contact *next;
    struct contact *prev;

} contact;

int AddContact(contact *newContact);

int EditContact(const char *name, const char *surname, int field, const char *value, int age);

int DeleteContact(const char *name, const char *surname);

contact *WatchContact(void);

void Menu(void);

#endif