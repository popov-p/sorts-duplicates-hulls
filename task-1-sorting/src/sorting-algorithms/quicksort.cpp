#include "quicksort.h"

int64_t partition(uint8_t arr[], int64_t low, int64_t high) {
    int64_t pivot = arr[low];
    int64_t i = low - 1;
    int64_t j = high + 1;

    while (1) {
        do {
            i++;
        } while (arr[i] < pivot);

        do {
            j--;
        } while (arr[j] > pivot);


        if (i >= j)
            return j;

        int temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }
}

void quickSort(uint8_t arr[], int64_t low, int64_t high) {
    if (low < high) {
        int64_t p = partition(arr, low, high);
        quickSort(arr, low, p);
        quickSort(arr, p + 1, high);
    }
}
