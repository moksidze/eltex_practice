#include "queue.h"

Queue* create_queue(void) {
    Queue* q = malloc(sizeof(Queue));
    if (q == NULL) {
        printf("Ошибка!\n");
        return NULL;
    }
    q->head = NULL;
    q->size = 0;
    return q;
}

void enqueue(Queue* q, int priority, const char* name) {
    if (priority < 0 || priority > 255) {
        printf("Ошибка: 0 или 255!\n");
        return;
    }

    Item* new_item = malloc(sizeof(Item));
    if (new_item == NULL) {
        printf("Ошибка!\n");
        return;
    }
    new_item->priority = priority;
    strcpy(new_item->name, name);
    new_item->next = NULL;

    if (q->head == NULL) {
        q->head = new_item;
        q->size++;
        return;
    }

    if (priority > q->head->priority) {
        new_item->next = q->head;
        q->head = new_item;
        q->size++;
        return;
    }

    Item* current = q->head;
    while (current->next != NULL && current->next->priority >= priority) {
        current = current->next;
    }

    new_item->next = current->next;
    current->next = new_item;
    q->size++;
}

char* dequeue_first(Queue* q) {
    if (is_empty(q)) {
        printf("Ошибка!\n");
        return NULL;
    }

    Item* temp = q->head;
    char* name = malloc(strlen(temp->name) + 1);
    if (name != NULL) {
        strcpy(name, temp->name);
    }

    q->head = q->head->next;
    free(temp);
    q->size--;

    return name;
}

char* dequeue_by_priority(Queue* q, int priority) {
    if (is_empty(q)) {
        printf("Ошибка!\n");
        return NULL;
    }

    if (q->head->priority == priority) {
        return dequeue_first(q);
    }

    Item* current = q->head;
    while (current->next != NULL && current->next->priority != priority) {
        current = current->next;
    }

    if (current->next == NULL) {
        printf("Ошибка:не найден %d !\n", priority);
        return NULL;
    }

    Item* temp = current->next;
    char* name = malloc(strlen(temp->name) + 1);
    if (name != NULL) {
        strcpy(name, temp->name);
    }

    current->next = temp->next;
    free(temp);
    q->size--;

    return name;
}

char* dequeue_by_min_priority(Queue* q, int min_priority) {
    if (is_empty(q)) {
        return NULL;
    }

    Item* check = q->head;
    int found = 0;
    while (check != NULL) {
        if (check->priority >= min_priority) {
            found = 1;
            break;
        }
        check = check->next;
    }

    if (!found) {
        return NULL;
    }

    Item* current = q->head;
    Item* prev = NULL;       
    Item* last_good = NULL;  
    Item* before_last = NULL; 

    while (current != NULL && current->priority >= min_priority) {
        before_last = prev;
        prev = current;
        last_good = current;
        current = current->next;
    }

    if (last_good == NULL) {
        return NULL;
    }

    char* name = malloc(strlen(last_good->name) + 1);
    if (name != NULL) {
        strcpy(name, last_good->name);
    }

    if (before_last == NULL) {
        q->head = last_good->next;
    }
    else {
        before_last->next = last_good->next;
    }

    free(last_good);
    q->size--;

    return name;
}

int is_empty(Queue* q) {
    return (q == NULL || q->head == NULL);
}

int get_queue_size(Queue* q) {
    if (q == NULL) {
        return 0;
    }
    return q->size;
}

void print_queue(Queue* q) {
    if (is_empty(q)) {
        printf("Очередь пуста!\n");
        return;
    }

    printf("Очередь: ");
    Item* current = q->head;
    while (current != NULL) {
        printf("[%d - %s] ", current->priority, current->name);
        current = current->next;
    }
    printf("\n");
    printf("Размер очереди: %d\n", q->size);
}

void clear_queue(Queue* q) {
    if (q == NULL) {
        return;
    }

    while (!is_empty(q)) {
        char* name = dequeue_first(q);
        if (name != NULL) {
            free(name);
        }
    }
    free(q);
}

void show_menu(void) {
    printf("\nОчередь с приоритетом:\n");
    printf("  1. Добавить новый элемент\n");
    printf("  2. Извлечь первый элемент\n");
    printf("  3. Извлечь элемент с заданным приоритетом\n");
    printf("  4. Извлечь элемент с приоритетом >= заданного\n");
    printf("  5. Показать очередь\n");
    printf("  6. Тест\n");
    printf("  0. Выход\n\n");
}

void add_element(Queue* q) {
    int priority;
    char name[MAX_NAME_LEN];

    printf("Введите приоритет (0-255): ");
    if (scanf("%d", &priority) != 1) {
        printf("Некорректный ввод!\n");
        while (getchar() != '\n');
        return;
    }

    if (priority < 0 || priority > 255) {
        printf("Ошибка: приоритет должен быть в диапазоне от 0 до 255!\n");
        return;
    }

    printf("Введите имя: ");
    scanf("%s", name);

    enqueue(q, priority, name);
    printf("Элемент [приоритет=%d, имя=%s] добавлен!\n", priority, name);
    printf("Размер очереди: %d\n", get_queue_size(q));
}

void extract_first(Queue* q) {
    if (is_empty(q)) {
        printf("Очередь пуста!\n");
        return;
    }

    Item* temp = q->head;
    printf("\nПервый элемент: приоритет=%d, имя=%s\n", temp->priority, temp->name);

    char* name = dequeue_first(q);
    if (name != NULL) {
        printf("Элемент извлечён! Имя: %s\n", name);
        printf("Текущий размер очереди: %d\n", get_queue_size(q));
        free(name);
    }
}

void extract_by_priority(Queue* q) {
    if (is_empty(q)) {
        printf("Очередь пуста!\n");
        return;
    }

    int priority;
    printf("Введите приоритет (0-255): ");
    if (scanf("%d", &priority) != 1) {
        printf("Некорректный ввод!\n");
        while (getchar() != '\n');
        return;
    }

    if (priority < 0 || priority > 255) {
        printf("Ошибка: приоритет должен быть в диапазоне от 0 до 255!\n");
        return;
    }

    char* name = dequeue_by_priority(q, priority);
    if (name != NULL) {
        printf("Элемент извлечён! Имя: %s\n", name);
        printf("Текущий размер очереди: %d\n", get_queue_size(q));
        free(name);
    }
}

void extract_by_min_priority(Queue* q) {
    if (is_empty(q)) {
        printf("Очередь пуста!\n");
        return;
    }

    int min_priority;
    printf("Введите минимальный приоритет: ");
    if (scanf("%d", &min_priority) != 1) {
        printf("Некорректный ввод!\n");
        while (getchar() != '\n');
        return;
    }

    if (min_priority < 0 || min_priority > 255) {
        printf("Ошибка: приоритет должен быть в диапазоне от 0 до 255!\n");
        return;
    }

    char* name = dequeue_by_min_priority(q, min_priority);
    if (name != NULL) {
        printf("Элемент извлечён! Имя: %s\n", name);
        printf("Текущий размер очереди: %d\n", get_queue_size(q));
        free(name);
    }
}

void show_queue(Queue* q) {
    printf("Текущее состояние очереди:\n");
    print_queue(q);
}