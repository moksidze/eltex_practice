#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "calc.h"

struct command
{
    char *name;
    double (*funk)(double a, double b);
};

int main(void)
{   
    int x;
    double a, b;

    struct command menu[] = {
        {"Сложение чисел", AdditionValues},
        {"Вычитание чисел", SubtractValues},
        {"Умножение чисел", MultiplyValues},
        {"Деление чисел", DivideValues}
    };

    system("clear");
    printf("Введите числа через пробел:");
    scanf("%lf %lf", &a, &b);

    int length = sizeof(menu) / sizeof(menu[0]);

    for(int i = 0; i < length; i++)
    {
        printf("%d.%s\n", i + 1, menu[i].name);
    }

    printf("\nВыберите пункт меню:");
    scanf("%d", &x);
    
    if(x > 0 && x <= length)
    {
        menu[x - 1].funk(a, b);
        printf("Результат: %.2lf\n", menu[x - 1].funk(a, b));
    }

    return 0;
}
