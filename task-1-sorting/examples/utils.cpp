#include "utils.h"

#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <assert.h>

void write_random_uint8_to_file(const char* filename, size_t n) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        perror("Не смогли открыть файл");
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
        perror("Не смогли открыть файл");
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);
    long filesize = ftell(file);
    rewind(file);

    if (filesize < 0) {
        perror("ftell не отработал");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    uint8_t* buffer = (uint8_t*)malloc(filesize);
    if (!buffer) {
        perror("malloc завершился с ошибкой");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    size_t read_count = fread(buffer, 1, filesize, file);
    if (read_count != (size_t)filesize) {
        perror("fread завершился с ошибкой");
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
    for (size_t i = 1; i < size; ++i) {
        // printf("%u ", data[i]);
        assert(data[i - 1] <= data[i] && "Массив не отсортирован");
    }
}

