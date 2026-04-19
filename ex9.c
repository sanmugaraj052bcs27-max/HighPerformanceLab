#include <stdio.h>
#include <omp.h>
#include <math.h>

// Function to check prime
int isPrime(int num) {
    if (num < 2) return 0;
    for (int i = 2; i <= sqrt(num); i++) {
        if (num % i == 0)
            return 0;
    }
    return 1;
}

int main() {
    int n;
    int count = 0;

    printf("Enter value of n: ");
    scanf("%d", &n);

    #pragma omp parallel for reduction(+:count)
    for (int i = 1; i <= n; i++) {
        if (isPrime(i)) {
            count++;
        }
    }

    printf("Total number of primes between 1 and %d = %d\n", n, count);

    return 0;
}

