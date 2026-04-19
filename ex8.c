#include <stdio.h>
#include <omp.h>

#define SIZE 10

int main() {
    int arr[SIZE] = {12, 45, 7, 89, 23, 56, 91, 3, 67, 34};
    int max = arr[0];

    #pragma omp parallel for reduction(max:max)
    for (int i = 0; i < SIZE; i++) {
        if (arr[i] > max) {
            max = arr[i];
        }
    }

    printf("Maximum value = %d\n", max);
    return 0;
}
