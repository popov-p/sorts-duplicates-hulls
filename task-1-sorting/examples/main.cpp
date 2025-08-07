#include "quicksort.h"
#include "utils.h"
#include <stdio.h>

int main() {
    // uint8_t arr[] = {10, 7, 8, 9, 1, 5};
    // int64_t n = sizeof(arr) / sizeof(arr[0]);

    // quickSort(arr, 0, n - 1);
    // print_array(arr, n);

    const char* filename = "random_data.bin";
    size_t n = 100;

    write_random_uint8_to_file(filename, n);
    printf("Записано %zu случайных байт в файл %s\n", n, filename);



    size_t size = 0;
    uint8_t* data = read_uint8_from_file(filename, &size);
    printf("Считано %zu байт из файла %s\n", size, filename);

    quickSort(data, 0, (int64_t)size - 1);

    assert_sorted_uint8(data, (int64_t)size - 1);
    free(data);

    return 0;
}
