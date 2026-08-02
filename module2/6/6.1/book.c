#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "book.h"

contact *head = NULL;

int AddContact(contact *newContact)
{
    contact *node;
    contact *current;
    contact *previous = NULL;

    if (newContact == NULL)
    {
        return 0;
    }

    node = malloc(sizeof(contact));

    if (node == NULL)
    {
        return 0;
    }

    *node = *newContact;

    node->next = NULL;
    node->prev = NULL;

    current = head;

    while (current != NULL)
    {
        int cmp = strcmp(current->surname, node->surname);

        if (cmp < 0)
        {
            previous = current;
            current = current->next;
        }
        else if (cmp == 0)
        {
            cmp = strcmp(current->name, node->name);

            if (cmp < 0)
            {
                previous = current;
                current = current->next;
            }
            else
            {
                break;
            }
        }
        else
        {
            break;
        }
    }

    if (previous == NULL)
    {
        node->next = head;
        node->prev = NULL;

        if (head != NULL)
        {
            head->prev = node;
        }

        head = node;
    }
    else
    {
        node->next = current;
        node->prev = previous;

        previous->next = node;

        if (current != NULL)
        {
            current->prev = node;
        }
    }

    return 1;
}

int EditContact(const char *name, const char *surname, int field, const char *value, int age)
{
    contact *current = head;

    while (current != NULL)
    {
        if (strcmp(current->name, name) == 0 &&
            strcmp(current->surname, surname) == 0)
        {
            break;
        }

        current = current->next;
    }

    if (current == NULL)
    {
        return 0;
    }

    switch (field)
    {
        case 1:
            strcpy(current->name, value);
            break;

        case 2:
            strcpy(current->surname, value);
            break;

        case 3:
            strcpy(current->patronymic, value);
            break;

        case 4:
            current->age = age;
            break;

        case 5:
            strcpy(current->work, value);
            break;

        case 6:
            strcpy(current->position, value);
            break;

        case 7:
            strcpy(current->phone, value);
            break;

        case 8:
            strcpy(current->email, value);
            break;

        case 9:
            strcpy(current->link, value);
            break;

        case 10:
            strcpy(current->messenger, value);
            break;

        default:
            return -1;
    }

    return 1;
}

int DeleteContact(const char *name, const char *surname)
{
    contact *current = head;

    while (current != NULL)
    {
        if (strcmp(current->name, name) == 0 &&
            strcmp(current->surname, surname) == 0)
        {
            break;
        }

        current = current->next;
    }

    if (current == NULL)
    {
        return 0;
    }

    if (current->prev != NULL)
    {
        current->prev->next = current->next;
    }
    else
    {
        head = current->next;
    }

    if (current->next != NULL)
    {
        current->next->prev = current->prev;
    }

    free(current);

    return 1;
}

contact *WatchContact(void)
{
    return head;
}