#include "queue.h"

void run_tests(void) {
    printf("\nТестирование:\n\n");

    Queue* q = create_queue();
    if (q == NULL) {
        return;
    }

    printf("1. Генерация сообщений\n\n");

    const char* senders[] = { "Server", "Client", "Router", "DNS", "Mail" };
    int total = 15;

    int high_count = 0;
    int medium_count = 0;
    int low_count = 0;

    for (int i = 0; i < total; i++) {
        int priority;
        char msg[50];

        // Генерация приоритета:
        // 20% сообщений имеют высокий приоритет (200–255),
        // 30% — средний (100–199),
        // 50% — низкий (0–99).
        int rand_type = rand() % 100;

        if (rand_type < 20) {
            priority = 200 + rand() % 56;
            high_count++;
        }
        else if (rand_type < 50) {
            priority = 100 + rand() % 100;
            medium_count++;
        }
        else {
            priority = rand() % 100;
            low_count++;
        }

        sprintf(msg, "%s_%d", senders[rand() % 5], i + 1);

        // Добавление сообщения в очередь
        enqueue(q, priority, msg);

        printf("  Получено сообщение: приоритет=%d, имя=%s\n", priority, msg);
    }

    printf("\nСгенерировано сообщений:\n");
    printf("  Высокий приоритет (200–255): %d\n", high_count);
    printf("  Средний приоритет (100–199): %d\n", medium_count);
    printf("  Низкий приоритет (0–99): %d\n", low_count);

    printf("\n");

    printf("Очередь перед обработкой:\n");
    print_queue(q);

    printf("\n");

    printf("2. Обработка сообщений\n\n");

    int processed_high = 0;
    int processed_medium = 0;
    int processed_low = 0;

    // Обработка сообщений с высоким приоритетом
    printf("2.1 Сообщения с высоким приоритетом (>=200):\n");

    while (1) {
        char* name = dequeue_by_min_priority(q, 200);

        if (name == NULL)
            break;

        printf("  Обработано сообщение: %s (Высокий)\n", name);

        free(name);
        processed_high++;
    }

    printf("  Обработано сообщений высокого приоритета: %d\n", processed_high);

    printf("\n");

    // Обработка сообщений со средним приоритетом
    printf("2.2 Сообщения со средним приоритетом (>=100):\n");

    while (1) {
        char* name = dequeue_by_min_priority(q, 100);

        if (name == NULL)
            break;

        printf("  Обработано сообщение: %s (Средний)\n", name);

        free(name);
        processed_medium++;
    }

    printf("  Обработано сообщений среднего приоритета: %d\n", processed_medium);

    printf("\n");

    // Обработка оставшихся сообщений
    printf("2.3 Сообщения с низким приоритетом:\n");

    while (1) {
        char* name = dequeue_first(q);

        if (name == NULL)
            break;

        printf("  Обработано сообщение: %s (Низкий)\n", name);

        free(name);
        processed_low++;
    }

    printf("  Обработано сообщений низкого приоритета: %d\n", processed_low);

    printf("\nСтатистика\n\n");

    printf("Всего сгенерировано сообщений: %d\n\n", total);

    printf("По уровням приоритета:\n");

    printf("  Высокий (>=200): %d (%.1f%%)\n",
           processed_high,
           (float)processed_high / total * 100);

    printf("  Средний (>=100): %d (%.1f%%)\n",
           processed_medium,
           (float)processed_medium / total * 100);

    printf("  Низкий: %d (%.1f%%)\n",
           processed_low,
           (float)processed_low / total * 100);

    printf("\n");

    printf("Порядок обработки сообщений:\n");
    printf("Высокий приоритет → Средний приоритет → Низкий приоритет\n\n");

    // Освобождение памяти, выделенной под очередь
    clear_queue(q);
}