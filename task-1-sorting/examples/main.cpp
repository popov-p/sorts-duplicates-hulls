#include "utils.h"
#include "task_queue.h"
#include "quicksort.h"

#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    size_t n = 100;
    uint8_t* arr = (uint8_t*)malloc(n * sizeof(uint8_t));
    if (!arr) {
        perror("malloc failed");
        return 1;
    }

    for (size_t i = 0; i < n; i++)
        arr[i] = rand() % 256;

    TaskQueue* queue = task_queue_create();

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    clock_gettime(CLOCK_MONOTONIC, &start);
    parallel_quickSort(queue, arr, 0, n - 1, 12, 10000);
    clock_gettime(CLOCK_MONOTONIC, &end);

    double elapsed_sec = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Sort completed in %.3f seconds.\n", elapsed_sec);

    write_stats_to_csv(12, 10000, n, elapsed_sec, queue->max_length);

    assert_sorted_uint8(arr, n);

    printf("Array sorted correctly.\n");
    free(arr);
    task_queue_destroy(queue);
    return 0;
}
