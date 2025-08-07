#ifndef GEN_UTILS_H
#define GEN_UTILS_H

#include <stdint.h>
#include <stdlib.h>

void write_random_uint8_to_file(const char* filename, size_t n);
uint8_t* read_uint8_from_file(const char* filename, size_t* out_size);
void print_array(uint8_t* arr, int size);
void assert_sorted_uint8(const uint8_t* data, size_t size);

#endif
