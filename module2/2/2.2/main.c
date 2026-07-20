#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void Menu();
void AdditionValues(double a, double b);
void SubtractValues(double a, double b);
void MultiplyValues(double a, double b);
void DivideValues(double a, double b);

int main(void)
{   
    Menu();

    return 0;
}

void Menu()
{
    int x;
    double a, b;

    system("clear");
    printf("Введите числа через пробел:");
    scanf("%lf %lf", &a, &b);

    system("clear");
    printf("*МЕНЮ*\n\n");
    printf("1.Сложение чисел\n");
    printf("2.Вычетание чисел\n");
    printf("3.Умножение чисел\n");
    printf("4.Деление чисел\n");
    printf("\nВыберите пункт меню:");

    scanf("%d", &x);
    switch(x)
    {   
        case 1:
            system("clear");
            printf("*Сложение чисел*\n\n");
            AdditionValues(a, b);
            break;
        case 2:
            system("clear");
            printf("*Вычетание чисел*\n\n");
            SubtractValues(a, b);
            break;
        case 3:
            system("clear");
            printf("*Умножение чисел*\n\n");
            MultiplyValues(a, b);
            break;
        case 4:
            system("clear");
            printf("*Деление чисел*\n\n");
            DivideValues(a, b);
            break;
        default:
            printf("*Вы вышли из меню*\n\n");
            break;
    }
}

void AdditionValues(double a, double b)
{   
    char x;

    printf("%0.2lf + %0.2lf = %0.2lf", a, b, a + b);

    printf("\nДля возвращения в меню введите любой символ:");
    scanf(" %c", &x);
    Menu();
}

void SubtractValues(double a, double b)
{
    char x;

    printf("%0.2lf - %0.2lf = %0.2lf", a, b, a - b);

    printf("\nДля возвращения в меню введите любой символ:");
    scanf(" %c", &x);
    Menu();
}

void MultiplyValues(double a, double b)
{
    char x;

    printf("%0.2lf * %0.2lf = %0.2lf", a, b, a * b);

    printf("\nДля возвращения в меню введите любой символ:");
    scanf(" %c", &x);
    Menu();
}

void DivideValues(double a, double b)
{   
    char x;

    if(b == 0)
    {
        printf("Делить на ноль нельзя!");

        printf("\nДля возвращения в меню введите любой символ:");
        scanf(" %c", &x);
        Menu();
    }

    printf("%0.2lf / %0.2lf = %0.2lf", a, b, a / b);

    printf("\nДля возвращения в меню введите любой символ:");
    scanf(" %c", &x);
    Menu();
}
