#include "bucketsort.h"

#include <stdlib.h>
#include <pthread.h>

Node* insert(Node* head, uint8_t value) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->data = value;
    node->next = head;
    return node;
}

Node* InsertionSort(Node* list) {
    if (!list || !list->next) return list;

    Node* sorted = list;
    Node* k = list->next;
    sorted->next = NULL;

    while (k) {
        Node* ptr;
        if (k->data < sorted->data) {
            Node* tmp = k;
            k = k->next;
            tmp->next = sorted;
            sorted = tmp;
            continue;
        }

        for (ptr = sorted; ptr->next && ptr->next->data < k->data; ptr = ptr->next);

        Node* tmp = k;
        k = k->next;
        tmp->next = ptr->next;
        ptr->next = tmp;
    }

    return sorted;
}

void* thread_func(void* arg) {
    ThreadArgs* args = (ThreadArgs*)arg;
    Node** my_buckets = args->local_buckets[args->thread_id];

    for (size_t i = args->start; i < args->end; ++i) {
        uint8_t value = args->input[i];
        uint8_t index = value / args->interval;
        if (index >= args->n_bucket) index = args->n_bucket - 1;
        my_buckets[index] = insert(my_buckets[index], value);
    }

    for (uint8_t i = 0; i < args->n_bucket; ++i)
        my_buckets[i] = InsertionSort(my_buckets[i]);

    return NULL;
}

void BucketSortParallel(uint8_t arr[], size_t n_array, uint8_t n_bucket, uint8_t interval, uint8_t num_threads) {
    pthread_t threads[num_threads];
    ThreadArgs args[num_threads];

    Node*** local_buckets = (Node***)malloc(sizeof(Node**) * num_threads);
    for (uint8_t t = 0; t < num_threads; ++t) {
        local_buckets[t] = (Node**)malloc(sizeof(Node*) * n_bucket);
        for (uint8_t i = 0; i < n_bucket; ++i)
            local_buckets[t][i] = NULL;
    }

    size_t chunk = (n_array + num_threads - 1) / num_threads;

    for (uint8_t t = 0; t < num_threads; ++t) {
        args[t].thread_id = t;
        args[t].input = arr;
        args[t].start = t * chunk;
        args[t].end = (t + 1) * chunk;
        if (args[t].end > n_array) args[t].end = n_array;
        args[t].local_buckets = local_buckets;
        args[t].n_bucket = n_bucket;
        args[t].interval = interval;
        pthread_create(&threads[t], NULL, thread_func, &args[t]);
    }

    for (uint8_t t = 0; t < num_threads; ++t)
        pthread_join(threads[t], NULL);

    size_t idx = 0;
    for (uint8_t b = 0; b < n_bucket; ++b) {
        for (uint8_t t = 0; t < num_threads; ++t) {
            Node* node = local_buckets[t][b];
            while (node) {
                arr[idx++] = node->data;
                node = node->next;
            }
        }
    }

    for (uint8_t t = 0; t < num_threads; ++t) {
        for (uint8_t b = 0; b < n_bucket; ++b) {
            Node* node = local_buckets[t][b];
            while (node) {
                Node* tmp = node;
                node = node->next;
                free(tmp);
            }
        }
        free(local_buckets[t]);
    }
    free(local_buckets);
}
