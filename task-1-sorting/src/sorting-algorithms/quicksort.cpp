#include "quicksort.h"

#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdatomic.h>

#define MAX_THREADS 8
#define MIN_SIZE_FOR_THREAD 1000

typedef struct {
    uint8_t* arr;
    int64_t low;
    int64_t high;
} QuickSortArgs;

atomic_int active_threads = 1; // Счётчик активных потоков, стартует с 1 (главный поток)

int64_t partition(uint8_t arr[], int64_t low, int64_t high) {
    int64_t pivot = arr[low];
    int64_t i = low - 1;
    int64_t j = high + 1;

    while (1) {
        do { i++; } while (arr[i] < pivot);
        do { j--; } while (arr[j] > pivot);

        if (i >= j)
            return j;

        int temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }
}

void quickSort(uint8_t arr[], int64_t low, int64_t high) {
    if (low < high) {
        int64_t p = partition(arr, low, high);
        quickSort(arr, low, p);
        quickSort(arr, p + 1, high);
    }
}

void* quicksort_thread(void* arg) {
    quicksort_args_t* args = (quicksort_args_t*)arg;
    int64_t size = args->high - args->low + 1;

    if (size <= MIN_SIZE_FOR_THREAD) {
        quickSort(args->arr, args->low, args->high);
        free(args);
        atomic_fetch_sub(&active_threads, 1);
        return NULL;
    }

    int64_t p = partition(args->arr, args->low, args->high);

    pthread_t left_thread;
    int left_thread_created = 0;

    // Попытка создать поток для левой части, если можно
    if (atomic_load(&active_threads) < MAX_THREADS) {
        quicksort_args_t* left_args = (quicksort_args_t*)malloc(sizeof(QuickSortArgs));
        if (!left_args) {
            perror("malloc failed");
            quickSort(args->arr, args->low, p);
        } else {
            left_args->arr = args->arr;
            left_args->low = args->low;
            left_args->high = p;

            atomic_fetch_add(&active_threads, 1);
            if (pthread_create(&left_thread, NULL, quicksort_thread, left_args) == 0) {
                left_thread_created = 1;
            } else {
                atomic_fetch_sub(&active_threads, 1);
                quickSort(args->arr, args->low, p);
                free(left_args);
            }
        }
    } else {
        quickSort(args->arr, args->low, p);
    }

    // Сортируем правую часть в текущем потоке
    quickSort(args->arr, p + 1, args->high);

    if (left_thread_created) {
        pthread_join(left_thread, NULL);
    }

    free(args);
    atomic_fetch_sub(&active_threads, 1);
    return NULL;
}

// Вспомогательная функция для запуска сортировки в новом потоке
void parallel_quickSort(uint8_t arr[], int64_t low, int64_t high) {
    quicksort_args_t* args = (quicksort_args_t*)malloc(sizeof(quicksort_args_t));
    if (!args) {
        perror("malloc failed");
        quickSort(arr, low, high);
        return;
    }

    args->arr = arr;
    args->low = low;
    args->high = high;

    quicksort_thread(args);  // Запускаем сразу в текущем потоке, порождая другие потоки внутри
}

// Тестирование
void print_array(uint8_t* arr, int64_t size) {
    for (int64_t i = 0; i < size; i++)
        printf("%d ", arr[i]);
    printf("\n");
}

int main() {
    size_t n = 10000;
    uint8_t* arr = (uint8_t*)malloc(n * sizeof(uint8_t));
    if (!arr) {
        perror("malloc failed");
        return 1;
    }

    // Заполняем случайными данными
    for (size_t i = 0; i < n; i++) {
        arr[i] = rand() % 256;
    }

    printf("Before sorting (first 20 elements):\n");
    print_array(arr, 20);

    parallel_quickSort(arr, 0, n - 1);

    printf("After sorting (first 20 elements):\n");
    print_array(arr, 20);

    // Проверка что отсортировано
    for (size_t i = 1; i < n; i++) {
        if (arr[i-1] > arr[i]) {
            fprintf(stderr, "Array not sorted at index %zu!\n", i);
            free(arr);
            return 1;
        }
    }

    free(arr);
    return 0;
}
