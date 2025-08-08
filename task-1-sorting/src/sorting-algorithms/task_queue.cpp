#include "task_queue.h"

#include <stdlib.h>
#include <stdio.h>

TaskQueue* task_queue_create(void) {
    TaskQueue* queue = (TaskQueue*)malloc(sizeof(TaskQueue));
    if (!queue) return NULL;

    queue->head = NULL;
    queue->tail = NULL;
    pthread_mutex_init(&queue->mutex, NULL);
    pthread_cond_init(&queue->cond, NULL);
    queue->stop = 0;
    queue->current_length = 0;
    queue->max_length = 0;
    atomic_init(&queue->active_tasks, 0);
    return queue;
}

void task_queue_destroy(TaskQueue* queue) {
    pthread_mutex_lock(&queue->mutex);
    struct Task* task = queue->head;
    while (task) {
        struct Task* next = task->next;
        free(task);
        task = next;
    }
    pthread_mutex_unlock(&queue->mutex);

    pthread_mutex_destroy(&queue->mutex);
    pthread_cond_destroy(&queue->cond);
    free(queue);
}

void enqueue_task(TaskQueue* queue, QuickSortArgs args) {
    Task* task = (Task*)malloc(sizeof(Task));
    if (!task) {
        perror("Malloc failed in enqueue_task");
        exit(1);
    }
    task->args = args;
    task->next = NULL;

    pthread_mutex_lock(&queue->mutex);
    if (queue->tail) {
        queue->tail->next = task;
        queue->tail = task;
    } else
        queue->head = queue->tail = task;

    queue->current_length++;
    if (queue->current_length > queue->max_length)
        queue->max_length = queue->current_length;


    int active = atomic_fetch_add(&queue->active_tasks, 1) + 1;
    (void)active;
    // printf("[Queue] Enqueue: low=%lld high=%lld active_tasks=%d current_length=%d\n",
    //        (long long)args.low, (long long)args.high, active, queue->current_length);
    // fflush(stdout);

    pthread_cond_signal(&queue->cond);
    pthread_mutex_unlock(&queue->mutex);
}

int dequeue_task(TaskQueue* queue, QuickSortArgs* out) {
    pthread_mutex_lock(&queue->mutex);
    while (!queue->head && !queue->stop) {
        // printf("[Queue] Waiting for tasks...\n");
        // fflush(stdout);
        pthread_cond_wait(&queue->cond, &queue->mutex);
    }

    if (queue->stop && !queue->head) {
        pthread_mutex_unlock(&queue->mutex);
        // printf("[Queue] Stop signal received, dequeue exiting.\n");
        // fflush(stdout);
        return 0;
    }
    Task* task = queue->head;
    queue->head = task->next;
    if (!queue->head)
        queue->tail = NULL;

    queue->current_length--;

    pthread_mutex_unlock(&queue->mutex);

    *out = task->args;
    free(task);
    return 1;
}

void task_queue_stop(TaskQueue* queue) {
    pthread_mutex_lock(&queue->mutex);
    queue->stop = 1;
    pthread_cond_broadcast(&queue->cond);
    pthread_mutex_unlock(&queue->mutex);
}

int task_queue_active_tasks(TaskQueue* queue) {
    return atomic_load(&queue->active_tasks);
}

int task_queue_max_length(TaskQueue* queue) {
    pthread_mutex_lock(&queue->mutex);
    int max_len = queue->max_length;
    pthread_mutex_unlock(&queue->mutex);
    return max_len;
}

void task_queue_finish_task(TaskQueue* queue) {
    int remaining = atomic_fetch_sub(&queue->active_tasks, 1) - 1;
    (void)remaining;
    // printf("[Queue] Finished task, remaining active_tasks=%d, currentTasks=%d\n",
    //        remaining, queue->current_length);
    // fflush(stdout);

    if (remaining == 0) {
        pthread_mutex_lock(&queue->mutex);
        pthread_cond_broadcast(&queue->cond);
        pthread_mutex_unlock(&queue->mutex);
        // printf("[Queue] All tasks finished, signaled main thread.\n");
    }
}
