#include "utils.h"

int main() {
    int num_threads_values[] = {256};
    // int64_t min_size_values[] = {100000, 200000, 500000, 10000000, 2000000};
    int64_t min_size_values[] = {1};

    size_t n = 100000;
    size_t out = 0;
    const char* input_data = "random_data.bin";

    write_random_uint8_to_file(input_data, n);
    uint8_t* original_buffer = read_uint8_from_file(input_data, &out);

    for (size_t i = 0; i < sizeof(num_threads_values)/sizeof(num_threads_values[0]); i++)
        for (size_t j = 0; j < sizeof(min_size_values)/sizeof(min_size_values[0]); j++) {
            // run_quicksort_with_params(original_buffer, out, num_threads_values[i], min_size_values[j]);
            run_bucket_sort_with_params(original_buffer, out, num_threads_values[i]);

        }

    free(original_buffer);
    return 0;
}
