#include "utils.h"
#include "task_queue.h"
#include "quicksort.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    int num_threads_values[] = {8};
    int64_t min_size_values[] = {500000};

    size_t n = 60000000;
    size_t out = 0;
    const char* input_data = "random_data.bin";

    write_random_uint8_to_file(input_data, n);
    uint8_t* buffer = read_uint8_from_file(input_data, &out);

    printf("Bytes read from file: %zu\n", out);

    for (size_t i = 0; i < sizeof(num_threads_values)/sizeof(num_threads_values[0]); i++) {
        for (size_t j = 0; j < sizeof(min_size_values)/sizeof(min_size_values[0]); j++) {
            int num_threads = num_threads_values[i];
            int64_t min_size_for_thread = min_size_values[j];
            TaskQueue* queue = task_queue_create();

            struct timespec start, end;

            clock_gettime(CLOCK_MONOTONIC, &start);
            parallel_quickSort(queue, buffer, 0, n - 1, num_threads, min_size_for_thread);
            clock_gettime(CLOCK_MONOTONIC, &end);

            double elapsed_sec = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
            printf("Sort completed in %.8f seconds.\n", elapsed_sec);
            // fflush(stdout);

            write_stats_to_csv(num_threads, min_size_for_thread, n, elapsed_sec, queue->max_length);

            assert_sorted_uint8(buffer, n);
            task_queue_destroy(queue);
        }
    }

    free(buffer);
    return 0;
}
