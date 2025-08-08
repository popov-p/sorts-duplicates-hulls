#include "quicksort.h"
#include "task_queue.h"

#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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

void* worker_thread(void* arg) {
    WorkerArgs* wargs = (WorkerArgs*)arg;
    TaskQueue* queue = wargs->queue;
    int min_size_for_thread = wargs->min_size_for_thread;

    QuickSortArgs task;

    while (dequeue_task(queue, &task)) {
        int64_t size = task.high - task.low + 1;
        // printf("[Worker %ld] Dequeued task: low=%ld high=%ld size=%ld\n",
        //        pthread_self(), task.low, task.high, size);
        // fflush(stdout);

        if (size <= min_size_for_thread) {
            // printf("[Worker %ld] Task size <= min_size (%d), sorting directly\n", pthread_self(), min_size_for_thread);
            // fflush(stdout);
            quickSort(task.arr, task.low, task.high);
            task_queue_finish_task(queue);
            continue;
        }

        int64_t p = partition(task.arr, task.low, task.high);
        // printf("[Worker %ld] Partition returned: %ld\n", pthread_self(), p);
        // fflush(stdout);
        // if (p <= task.low || p >= task.high) {
        //     printf("[Worker %ld] Partition index out of bounds, sorting directly\n", pthread_self());
        //     fflush(stdout);
        //     quickSort(task.arr, task.low, task.high);
        //     task_queue_finish_task(queue);
        //     continue;
        // }

        QuickSortArgs left = {task.arr, task.low, p};
        QuickSortArgs right = {task.arr, p + 1, task.high};

        // printf("[Worker %ld] Enqueuing left task: low=%ld high=%ld\n", pthread_self(), left.low, left.high);
        // fflush(stdout);
        enqueue_task(queue, left);
        // printf("[Worker %ld] Enqueuing right task: low=%ld high=%ld\n", pthread_self(), right.low, right.high);
        // fflush(stdout);
        enqueue_task(queue, right);

        task_queue_finish_task(queue);
    }
    return NULL;
}

void parallel_quickSort(TaskQueue* queue, uint8_t arr[], int64_t low, int64_t high, int8_t num_threads, int64_t min_size_for_thread) {
    if(!num_threads || !min_size_for_thread)
        exit(EXIT_FAILURE);

    pthread_t threads[num_threads];
    queue->stop = 0;

    WorkerArgs wargs = {queue, min_size_for_thread};

    for (size_t i = 0; i < num_threads; i++)
        pthread_create(&threads[i], NULL, worker_thread, &wargs);

    QuickSortArgs initial = {arr, low, high};
    enqueue_task(queue, initial);

    pthread_mutex_lock(&queue->mutex);
    while (task_queue_active_tasks(queue) > 0)
        pthread_cond_wait(&queue->cond, &queue->mutex);
    pthread_mutex_unlock(&queue->mutex);
    task_queue_stop(queue);

    for (int i = 0; i < num_threads; i++)
        pthread_join(threads[i], NULL);
}
