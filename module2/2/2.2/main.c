#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "calc.h"

int main(void)
{   
    int x;
    double a, b;

    system("clear");
    printf("Введите числа через пробел:");
    scanf("%lf %lf", &a, &b);

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
            printf("%.2lf + %.2lf = %.2lf\n", a, b, AdditionValues(a, b));
            break;
        case 2:
            system("clear");
            printf("*Вычетание чисел*\n\n");
            printf("%.2lf + %.2lf = %.2lf\n", a, b, SubtractValues(a, b));
            break;
        case 3:
            system("clear");
            printf("*Умножение чисел*\n\n");
            printf("%.2lf + %.2lf = %.2lf\n", a, b, MultiplyValues(a, b));
            break;
        case 4:
            system("clear");
            printf("*Деление чисел*\n\n");
            printf("%.2lf + %.2lf = %.2lf\n", a, b, DivideValues(a, b));
            break;
        default:
            printf("*Вы вышли из меню*\n\n");
            break;
    }

    return 0;
}
