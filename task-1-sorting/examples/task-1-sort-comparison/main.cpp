#include "utils.h"

#include <memory.h>
#include <stdio.h>

void test_quicksort() {
    int qsort_num_threads_values[] = {8};
    int64_t min_size_values[] = {100000, 200000, 500000, 10000000, 2000000};

    size_t qsort_n = 200000000;
    size_t qsort_out = 0;
    const char* qsort_input_data = "qsort_random_data.bin";

    write_random_uint8_to_file(qsort_input_data, qsort_n);
    uint8_t* qsort_original_buffer = read_uint8_from_file(qsort_input_data, &qsort_out);

    for (size_t i = 0; i < sizeof(qsort_num_threads_values)/sizeof(qsort_num_threads_values[0]); i++) {
        for (size_t j = 0; j < sizeof(min_size_values)/sizeof(min_size_values[0]); j++) {
            run_quicksort_with_params(qsort_original_buffer, qsort_n,
                                      qsort_num_threads_values[i], min_size_values[j]);
        }
    }

    free(qsort_original_buffer);
}

void test_bucketsort() {
    int bsort_num_threads_values[] = {12};
    uint8_t bsort_n_bucket_values[] = {12};
    uint8_t bsort_interval_values[] = {32};

    size_t bsort_n = 1000000;
    size_t bsort_out = 0;
    const char* bsort_input_data = "bsort_random_data.bin";

    write_random_uint8_to_file(bsort_input_data, bsort_n);
    uint8_t* original_buffer = read_uint8_from_file(bsort_input_data, &bsort_out);

    for (size_t i = 0; i < sizeof(bsort_num_threads_values) / sizeof(bsort_num_threads_values[0]); i++) {
        for (size_t j = 0; j < sizeof(bsort_n_bucket_values) / sizeof(bsort_n_bucket_values[0]); j++) {
            for (size_t k = 0; k < sizeof(bsort_interval_values) / sizeof(bsort_interval_values[0]); k++) {
                run_bucket_sort_with_params(original_buffer, bsort_out,
                                            bsort_n_bucket_values[j], bsort_interval_values[k],
                                            bsort_num_threads_values[i]);

            }
        }
    }

    free(original_buffer);
}


int main() {
    // test_quicksort();
    test_bucketsort();
    return 0;
}
