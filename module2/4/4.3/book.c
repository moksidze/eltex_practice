#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "book.h"

contact *root = NULL;

static int operationCount = 0;

static int Compare(contact *a, contact *b)
{
    int cmp = strcmp(a->surname, b->surname);

    if (cmp == 0)
        cmp = strcmp(a->name, b->name);

    return cmp;
}

static contact *Insert(contact *tree, contact *node)
{
    if (tree == NULL)
        return node;

    if (Compare(node, tree) < 0)
        tree->left = Insert(tree->left, node);
    else
        tree->right = Insert(tree->right, node);

    return tree;
}

static contact *FindMin(contact *tree)
{
    while (tree->left != NULL)
        tree = tree->left;

    return tree;
}

static int CountNodes(contact *tree)
{
    if (tree == NULL)
        return 0;

    return 1 + CountNodes(tree->left) + CountNodes(tree->right);
}

static void StoreInOrder(contact *tree, contact **array, int *index)
{
    if (tree == NULL)
        return;

    StoreInOrder(tree->left, array, index);

    array[*index] = tree;
    (*index)++;

    StoreInOrder(tree->right, array, index);
}

static contact *BuildBalanced(contact **array, int left, int right)
{
    if (left > right)
        return NULL;

    int mid = (left + right) / 2;

    contact *node = array[mid];

    node->left = BuildBalanced(array, left, mid - 1);
    node->right = BuildBalanced(array, mid + 1, right);

    return node;
}

static void BalanceTree(void)
{
    int count = CountNodes(root);

    if (count < 2)
        return;

    contact **array = malloc(count * sizeof(contact *));

    if (array == NULL)
        return;

    int index = 0;

    StoreInOrder(root, array, &index);

    root = BuildBalanced(array, 0, count - 1);

    free(array);
}

static contact *Find(contact *tree, const char *name, const char *surname)
{
    if (tree == NULL)
        return NULL;

    int cmp = strcmp(surname, tree->surname);

    if (cmp == 0)
        cmp = strcmp(name, tree->name);

    if (cmp == 0)
        return tree;

    if (cmp < 0)
        return Find(tree->left, name, surname);

    return Find(tree->right, name, surname);
}
int AddContact(contact *newContact)
{
    contact *node;

    if (newContact == NULL)
        return 0;

    node = malloc(sizeof(contact));

    if (node == NULL)
        return 0;

    *node = *newContact;

    node->left = NULL;
    node->right = NULL;

    root = Insert(root, node);

    operationCount++;

    if (operationCount >= 10)
    {
        BalanceTree();
        operationCount = 0;
    }

    return 1;
}

static contact *DeleteNode(contact *tree, const char *name, const char *surname)
{
    if (tree == NULL)
        return NULL;

    int cmp = strcmp(surname, tree->surname);

    if (cmp == 0)
        cmp = strcmp(name, tree->name);

    if (cmp < 0)
    {
        tree->left = DeleteNode(tree->left, name, surname);
    }
    else if (cmp > 0)
    {
        tree->right = DeleteNode(tree->right, name, surname);
    }
    else
    {
        if (tree->left == NULL)
        {
            contact *temp = tree->right;
            free(tree);
            return temp;
        }

        if (tree->right == NULL)
        {
            contact *temp = tree->left;
            free(tree);
            return temp;
        }

        contact *temp = FindMin(tree->right);

        strcpy(tree->name, temp->name);
        strcpy(tree->surname, temp->surname);
        strcpy(tree->patronymic, temp->patronymic);
        tree->age = temp->age;
        strcpy(tree->work, temp->work);
        strcpy(tree->position, temp->position);
        strcpy(tree->phone, temp->phone);
        strcpy(tree->email, temp->email);
        strcpy(tree->link, temp->link);
        strcpy(tree->messenger, temp->messenger);

        tree->right = DeleteNode(tree->right, temp->name, temp->surname);
    }

    return tree;
}

int EditContact(const char *name, const char *surname, int field, const char *value, int age)
{
    contact *current = Find(root, name, surname);

    if (current == NULL)
        return 0;

    if (field == 1 || field == 2)
    {
        contact temp = *current;

        root = DeleteNode(root, name, surname);

        if (field == 1)
            strcpy(temp.name, value);
        else
            strcpy(temp.surname, value);

        contact *node = malloc(sizeof(contact));

        if (node == NULL)
            return 0;

        *node = temp;
        node->left = NULL;
        node->right = NULL;

        root = Insert(root, node);

        operationCount++;

        if (operationCount >= 10)
        {
            BalanceTree();
            operationCount = 0;
        }

        return 1;
    }

    switch (field)
    {
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
    if (Find(root, name, surname) == NULL)
        return 0;

    root = DeleteNode(root, name, surname);

    operationCount++;

    if (operationCount >= 10)
    {
        BalanceTree();
        operationCount = 0;
    }

    return 1;
}

contact *WatchContact(void)
{
    return root;
}
