#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <math.h>

#define BUFFER_SIZE 5
#define NUM_WORKERS 3
#define MAX_NUM 100

int buffer[BUFFER_SIZE];
int in = 0, out = 0;

sem_t empty, full;
pthread_mutex_t mutex;
pthread_mutex_t file_mutex;

FILE *fp;

// Function to check prime
int isPrime(int n) {
    if (n < 2) return 0;
    for (int i = 2; i <= sqrt(n); i++) {
        if (n % i == 0) return 0;
    }
    return 1;
}

// Function to print factors
void printFactors(int n, char *result) {
    char temp[100] = "";
    for (int i = 1; i <= n; i++) {
        if (n % i == 0) {
            char buf[10];
            sprintf(buf, "%d ", i);
            strcat(temp, buf);
        }
    }
    sprintf(result, "%s", temp);
}

// Producer thread
void *producer(void *arg) {
    while (1) {
        int num = rand() % MAX_NUM + 1;

        sem_wait(&empty);
        pthread_mutex_lock(&mutex);

        buffer[in] = num;
        printf("Produced: %d\n", num);
        in = (in + 1) % BUFFER_SIZE;

        pthread_mutex_unlock(&mutex);
        sem_post(&full);

        sleep(1);
    }
}

// Worker thread
void *worker(void *arg) {
    while (1) {
        sem_wait(&full);
        pthread_mutex_lock(&mutex);

        int num = buffer[out];
        out = (out + 1) % BUFFER_SIZE;

        pthread_mutex_unlock(&mutex);
        sem_post(&empty);

        // Processing
        char result[256];
        char factors[100];

        printFactors(num, factors);

        sprintf(result,
                "Number: %d | %s | %s | Factors: %s\n",
                num,
                (num % 2 == 0) ? "Even" : "Odd",
                isPrime(num) ? "Prime" : "Not Prime",
                factors);

        // Write to file
        pthread_mutex_lock(&file_mutex);
        fprintf(fp, "%s", result);
        fflush(fp);
        pthread_mutex_unlock(&file_mutex);

        printf("Processed: %d\n", num);

        sleep(2);
    }
}

int main() {
    pthread_t prod, workers[NUM_WORKERS];

    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);

    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&file_mutex, NULL);

    fp = fopen("output.txt", "w");

    pthread_create(&prod, NULL, producer, NULL);

    for (int i = 0; i < NUM_WORKERS; i++) {
        pthread_create(&workers[i], NULL, worker, NULL);
    }

    pthread_join(prod, NULL);

    for (int i = 0; i < NUM_WORKERS; i++) {
        pthread_join(workers[i], NULL);
    }

    fclose(fp);

    sem_destroy(&empty);
    sem_destroy(&full);
    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&file_mutex);

    return 0;
}

