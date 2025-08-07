#ifndef QUICKSORT_H
#define QUICKSORT_H

#include "stdint.h"
#include <pthread.h>

#define MAX_THREADS 8
#define MIN_SIZE_FOR_THREAD 1000

int64_t partition(uint8_t arr[], int64_t low, int64_t high);
void quickSort(uint8_t arr[], int64_t low, int64_t high);

typedef struct {
    uint8_t* arr;
    int64_t low;
    int64_t high;
    int max_threads;
    int* current_threads;
    pthread_mutex_t* mutex;
} quicksort_args_t;

#endif
