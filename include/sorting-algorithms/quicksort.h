#ifndef QUICKSORT_H
#define QUICKSORT_H

#include "task_queue.h"
#include "stdint.h"

#include <pthread.h>

int64_t partition(uint8_t arr[], int64_t low, int64_t high);
void quickSort(uint8_t arr[], int64_t low, int64_t high);
void* worker_thread(void* arg);
void parallel_quickSort(TaskQueue* queue, uint8_t arr[], int64_t low, int64_t high, int8_t num_threads, int64_t min_size_for_thread);

typedef struct {
    uint8_t* arr;
    int64_t low;
    int64_t high;
    int max_threads;
    int* current_threads;
    pthread_mutex_t* mutex;
} quicksort_args_t;


typedef struct {
    TaskQueue* queue;
    const int64_t min_size_for_thread;
} WorkerArgs;

#endif
