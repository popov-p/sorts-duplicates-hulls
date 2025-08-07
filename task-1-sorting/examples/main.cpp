#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <unistd.h>

#ifdef _DEBUG
#define LOG_DEBUG(fmt, ...) \
do { fprintf(stderr, "[DEBUG] " fmt, ##__VA_ARGS__); } while (0)
#else
#define LOG_DEBUG(fmt, ...) do {} while (0)
#endif

#define NUM_THREADS 12
#define MIN_SIZE_FOR_THREAD 10000

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
} TaskQueue;

TaskQueue task_queue = {NULL, NULL, PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER, 0};

atomic_int active_tasks = 0;

void enqueue_task(QuickSortArgs args) {
    Task* task = (Task*)malloc(sizeof(Task));
    if (!task) {
        perror("malloc failed in enqueue_task");
        exit(1);
    }
    task->args = args;
    task->next = NULL;

    pthread_mutex_lock(&task_queue.mutex);
    if (task_queue.tail) {
        task_queue.tail->next = task;
        task_queue.tail = task;
    } else {
        task_queue.head = task_queue.tail = task;
    }
    atomic_fetch_add(&active_tasks, 1);
    pthread_cond_signal(&task_queue.cond);
    pthread_mutex_unlock(&task_queue.mutex);
}

int dequeue_task(QuickSortArgs* out) {
    pthread_mutex_lock(&task_queue.mutex);
    while (!task_queue.head && !task_queue.stop) {
        pthread_cond_wait(&task_queue.cond, &task_queue.mutex);
    }
    if (task_queue.stop) {
        pthread_mutex_unlock(&task_queue.mutex);
        return 0;
    }
    Task* task = task_queue.head;
    task_queue.head = task->next;
    if (!task_queue.head)
        task_queue.tail = NULL;
    pthread_mutex_unlock(&task_queue.mutex);

    *out = task->args;
    free(task);
    return 1;
}

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
    (void)arg;
    QuickSortArgs task;
    while (dequeue_task(&task)) {
        int64_t size = task.high - task.low + 1;
        if (size <= MIN_SIZE_FOR_THREAD) {
            quickSort(task.arr, task.low, task.high);
            atomic_fetch_sub(&active_tasks, 1);
            continue;
        }

        int64_t p = partition(task.arr, task.low, task.high);

        QuickSortArgs left = {task.arr, task.low, p};
        QuickSortArgs right = {task.arr, p + 1, task.high};

        enqueue_task(left);
        enqueue_task(right);

        atomic_fetch_sub(&active_tasks, 1);
    }
    LOG_DEBUG("[Thread %lu] Exiting\n", (unsigned long)pthread_self());
    return NULL;
}

void parallel_quickSort(uint8_t arr[], int64_t low, int64_t high) {
    pthread_t threads[NUM_THREADS];
    task_queue.stop = 0;

    for (int i = 0; i < NUM_THREADS; i++) {
        LOG_DEBUG("Worker thread started: pthread_self()=%lu\n", (unsigned long)pthread_self());
        pthread_create(&threads[i], NULL, worker_thread, NULL);
    }

    QuickSortArgs initial = {arr, low, high};
    enqueue_task(initial);

    while (atomic_load(&active_tasks) > 0) {
        usleep(1000);
    }

    pthread_mutex_lock(&task_queue.mutex);
    task_queue.stop = 1;
    pthread_cond_broadcast(&task_queue.cond);
    pthread_mutex_unlock(&task_queue.mutex);

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
        LOG_DEBUG("Joined worker thread %d with pthread_t %lu\n", i, (unsigned long)pthread_self());
    }
}

int main() {
    size_t n = 1000000000;
    uint8_t* arr = (uint8_t*)malloc(n * sizeof(uint8_t));
    if (!arr) {
        perror("malloc failed");
        return 1;
    }

    for (size_t i = 0; i < n; i++) {
        arr[i] = rand() % 256;
    }

    // printf("Just Generated the data.\n");
    // fflush(stdout);
    parallel_quickSort(arr, 0, n - 1);

    for (size_t i = 1; i < n; i++) {
        if (arr[i-1] > arr[i]) {
            fprintf(stderr, "Array not sorted at index %zu!\n", i);
            free(arr);
            return 1;
        }
    }

    printf("Array sorted correctly.\n");
    free(arr);
    return 0;
}
