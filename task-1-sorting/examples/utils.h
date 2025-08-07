#ifndef GEN_UTILS_H
#define GEN_UTILS_H

#include <stdint.h>
#include <stdlib.h>

#ifdef _DEBUG
#define LOG_DEBUG(fmt, ...) \
do { fprintf(stderr, "[DEBUG] " fmt, ##__VA_ARGS__); } while (0)
#else
#define LOG_DEBUG(fmt, ...) do {} while (0)
#endif

void write_random_uint8_to_file(const char* filename, size_t n);
uint8_t* read_uint8_from_file(const char* filename, size_t* out_size);
void print_array(uint8_t* arr, int size);
void write_stats_to_csv(int num_threads, int min_size, size_t n_elements, double time_sec, int max_queue_length);
void assert_sorted_uint8(const uint8_t* data, size_t size);

#endif
