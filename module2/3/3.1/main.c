#include <stdio.h>
#include <stdlib.h>
#include "mask.h"

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
    printf("1.Конвертер\n");
    printf("2.Права файла\n");
    printf("3.Изменить права\n");
    printf("\nВыберите пункт меню:");

    scanf("%d", &x);
    switch(x)
    {   
        case 1:
            system("clear");
            printf("*Конвертер*\n\n");
            Converter();
            break;
        case 2:
            system("clear");
            printf("*Права файла*\n\n");
            FileRights();
            break;
        case 3:
            system("clear");
            printf("*Изменить права*\n\n");
            ChangeRights();
            break;
        default:
            printf("*Вы вышли из меню*\n\n");
            break;
    }
}