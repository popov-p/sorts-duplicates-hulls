#ifndef TASK_QUEUE_H
#define TASK_QUEUE_H

#include <stdint.h>
#include <pthread.h>
#include <stdatomic.h>

typedef struct {
    uint8_t* arr;
    int64_t low;
    int64_t high;
} QuickSortArgs;

typedef struct Task {
    QuickSortArgs args;
    struct Task* next;
} Task;

typedef struct {
    Task* head;
    Task* tail;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int stop;

    int current_length;
    int max_length;

    atomic_int active_tasks;
} TaskQueue;

TaskQueue* task_queue_create(void);
void task_queue_destroy(TaskQueue* queue);

void enqueue_task(TaskQueue* queue, QuickSortArgs args);
int dequeue_task(TaskQueue* queue, QuickSortArgs* out);

void task_queue_stop(TaskQueue* queue);

int task_queue_active_tasks(TaskQueue* queue);

int task_queue_max_length(TaskQueue* queue);

void task_queue_finish_task(TaskQueue* queue);

#endif
