#include "bucketsort.h"

#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>


void print_list(Node* head) {
    while (head) {
        printf("%u ", head->data);
        head = head->next;
    }
    printf("\n");
}

Node* insert(Node* head, uint8_t value) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->data = value;
    node->next = head;
    return node;
}

Node* insertion_sort(Node* list) {
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
        my_buckets[i] = insertion_sort(my_buckets[i]);

    return NULL;
}

Node* merge_two_sorted_lists(Node* l1, Node* l2) {
    Node dummy;
    Node* tail = &dummy;
    dummy.next = NULL;

    while (l1 && l2) {
        if (l1->data <= l2->data) {
            tail->next = l1;
            l1 = l1->next;
        } else {
            tail->next = l2;
            l2 = l2->next;
        }
        tail = tail->next;
    }
    tail->next = (l1) ? l1 : l2;
    return dummy.next;
}

Node* merge_k_sorted_lists(Node** lists, size_t k) {
    if (k == 0) return NULL;
    if (k == 1) return lists[0];

    size_t interval = 1;
    while (interval < k) {
        for (size_t i = 0; i + interval < k; i += interval * 2) {
            lists[i] = merge_two_sorted_lists(lists[i], lists[i + interval]);
        }
        interval *= 2;
    }
    return lists[0];
}


void bucket_sort_parallel(uint8_t arr[], size_t n_array, uint8_t n_bucket, uint8_t interval, uint8_t num_threads) {
    if (n_bucket == 0) {
        fprintf(stderr, "Error: n_bucket must be > 0\n");
        exit(1);
    }

    if (interval == 0) {
        fprintf(stderr, "Error: interval must be > 0\n");
        exit(1);
    }

    if (num_threads == 0) {
        fprintf(stderr, "Error: num_threads must be > 0\n");
        exit(1);
    }

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
        Node** lists_to_merge = (Node**)malloc(sizeof(Node*) * num_threads);
        for (uint8_t t = 0; t < num_threads; ++t) {
            lists_to_merge[t] = local_buckets[t][b];
        }

        Node* merged = merge_k_sorted_lists(lists_to_merge, num_threads);

        free(lists_to_merge);

        Node* node = merged;
        while (node) {
            arr[idx++] = node->data;
            node = node->next;
        }

        node = merged;
        while (node) {
            Node* tmp = node;
            node = node->next;
            free(tmp);
        }
    }

    for (uint8_t t = 0; t < num_threads; ++t)
        free(local_buckets[t]);

    free(local_buckets);

}
