#ifndef GEN_UTILS_H
#define GEN_UTILS_H

#include <stdint.h>
#include <stdlib.h>

void write_random_uint8_to_file(const char* filename, size_t n);
uint8_t* read_uint8_from_file(const char* filename, size_t* out_size);
void print_array(uint8_t* arr, int size);
void assert_sorted_uint8(const uint8_t* data, size_t size);

void run_quicksort_with_params(uint8_t* original_buffer, size_t buffer_size, int64_t num_threads, int64_t min_size_for_thread);
void run_bucket_sort_with_params(uint8_t* original_buffer, size_t buffer_size,
                                 uint8_t n_bucket, uint8_t interval, int64_t num_threads);

#endif
