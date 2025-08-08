#ifndef BUCKET_SORT_H
#define BUCKET_SORT_H

#include <stdint.h>
#include <unistd.h>

typedef struct Node {
    uint8_t data;
    struct Node* next;
} Node;

typedef struct {
    uint8_t thread_id;
    uint8_t* input;
    size_t start;
    size_t end;
    Node*** local_buckets;
    uint8_t n_bucket;
    uint8_t interval;
} ThreadArgs;

Node* insert(Node* head, uint8_t value);
Node* insertion_sort(Node* list);
void* thread_func(void* arg);
void bucket_sort_parallel(uint8_t arr[], size_t n_array, uint8_t n_bucket, uint8_t interval, uint8_t num_threads);

#endif
