#include "queue.h"

int main() {
    srand(time(NULL));

    Queue* q = create_queue();
    if (q == NULL) {
        return 1;
    }

    int choice;

    printf("Очередь создана!\n\n");

    do {
        show_menu();
        printf("Введите значение: ");

        if (scanf("%d", &choice) != 1) {
            printf("Ошибка!\n");
            while (getchar() != '\n');
            continue;
        }

        switch (choice) {
        case 1:
            add_element(q);
            break;
        case 2:
            extract_first(q);
            break;
        case 3:
            extract_by_priority(q);
            break;
        case 4:
            extract_by_min_priority(q);
            break;
        case 5:
            show_queue(q);
            break;
        case 6:
            run_tests();
            break;
        case 0:
            printf("Выход из программы...\n");
            break;
        default:
            printf("Ошибка!\n");
            break;
        }
        printf("\n");

    } while (choice != 0);

    clear_queue(q);
    printf("Очередь пустая\n");

    return 0;
}