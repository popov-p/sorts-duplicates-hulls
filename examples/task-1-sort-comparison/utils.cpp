#include "task_queue.h"
#include "quicksort.h"
#include "bucketsort.h"
#include "utils.h"

#include <stdint.h>
#include <stdio.h>
#include <sys/stat.h>
#include <time.h>
#include <assert.h>
#include <memory.h>

void write_random_uint8_to_file(const char* filename, size_t n) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        perror("Unable to open file");
        exit(EXIT_FAILURE);
    }

    srand((unsigned)time(NULL));

    for (size_t i = 0; i < n; ++i) {
        uint8_t value = (uint8_t)(rand() % 256);
        fwrite(&value, sizeof(uint8_t), 1, file);
    }

    fclose(file);
}

uint8_t* read_uint8_from_file(const char* filename, size_t* out_size) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        perror("Unable to open file");
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);
    long filesize = ftell(file);
    rewind(file);

    if (filesize < 0) {
        perror("ftell failed");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    uint8_t* buffer = (uint8_t*)malloc(filesize);
    if (!buffer) {
        perror("malloc failed");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    size_t read_count = fread(buffer, 1, filesize, file);
    if (read_count != (size_t)filesize) {
        perror("fread failed");
        free(buffer);
        fclose(file);
        exit(EXIT_FAILURE);
    }

    fclose(file);
    *out_size = read_count;
    return buffer;
}


void print_array(uint8_t* arr, int size) {
    for (int i = 0; i < size; i++)
        printf("%u ", (uint8_t)arr[i]);
    printf("\n");
}

void assert_sorted_uint8(const uint8_t* data, size_t size) {
    for (size_t i = 1; i < size; ++i)
        assert(data[i - 1] <= data[i] && "Array is not sorted");
}

void write_stats_to_csv(int num_threads, int min_size, size_t n_elements, double time_sec, int max_queue_length) {
    const char* filename = "quicksort-stats.csv";
    struct stat st;
    int file_exists = stat(filename, &st) == 0;
    int write_header = !file_exists || st.st_size == 0;

    FILE* stats = fopen(filename, "a");
    if (!stats) {
        perror("Failed to open stats.csv");
        return;
    }

    if (write_header)
        fprintf(stats, "num_threads,min_size,n_elements,time_sec,max_queue_length\n");

    fprintf(stats, "%d,%d,%zu,%.8f,%d\n", num_threads, min_size, n_elements, time_sec, max_queue_length);
    fclose(stats);
}

void run_quicksort_with_params(uint8_t* original_buffer, size_t n, int64_t num_threads, int64_t min_size_for_thread) {
    uint8_t* buffer = (uint8_t*)malloc(n);
    if (!buffer) {
        perror("malloc failed");
        exit(1);
    }
    memcpy(buffer, original_buffer, n);

    TaskQueue* queue = task_queue_create();

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    parallel_quickSort(queue, buffer, 0, n - 1, num_threads, min_size_for_thread);
    clock_gettime(CLOCK_MONOTONIC, &end);

    double elapsed_sec = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Sort completed in %.8f seconds.\n", elapsed_sec);
    fflush(stdout);

    write_stats_to_csv(num_threads, min_size_for_thread, n, elapsed_sec, queue->max_length);

    assert_sorted_uint8(buffer, n);
    task_queue_destroy(queue);

    free(buffer);
}


void run_bucket_sort_with_params(uint8_t* original_buffer, size_t buffer_size,
                                 uint8_t n_bucket, uint8_t interval, int64_t num_threads) {
    uint8_t* buffer_copy = (uint8_t*)malloc(buffer_size);
    if (!buffer_copy) {
        perror("malloc failed in run_bucket_sort_with_params");
        exit(1);
    }
    memcpy(buffer_copy, original_buffer, buffer_size);

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    bucket_sort_parallel(buffer_copy, buffer_size, n_bucket, interval, num_threads);

    clock_gettime(CLOCK_MONOTONIC, &end);


    double elapsed_sec = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Sort completed in %.12f seconds.\n", elapsed_sec);
    fflush(stdout);

    assert_sorted_uint8(buffer_copy, buffer_size);

    free(buffer_copy);
}
