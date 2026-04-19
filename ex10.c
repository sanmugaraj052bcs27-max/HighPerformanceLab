#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

#define BUFFER_SIZE 5
#define MAX_LINE 256
#define NUM_PRODUCERS 2
#define NUM_CONSUMERS 2

char buffer[BUFFER_SIZE][MAX_LINE];
int in = 0, out = 0, count = 0;

omp_lock_t lock;
omp_lock_t file_lock;

FILE *output;

// Insert into buffer
void produce(char *line) {
    int done = 0;

    while (!done) {
        omp_set_lock(&lock);

        if (count < BUFFER_SIZE) {
            strcpy(buffer[in], line);
            in = (in + 1) % BUFFER_SIZE;
            count++;
            done = 1;
        }

        omp_unset_lock(&lock);
    }
}

// Remove from buffer
int consume(char *line) {
    int done = 0;

    while (!done) {
        omp_set_lock(&lock);

        if (count > 0) {
            strcpy(line, buffer[out]);
            out = (out + 1) % BUFFER_SIZE;
            count--;
            done = 1;
        }

        omp_unset_lock(&lock);
    }

    return 1;
}

int main() {
    char *files[NUM_PRODUCERS] = {"file1.txt", "file2.txt"};

    omp_init_lock(&lock);
    omp_init_lock(&file_lock);

    output = fopen("output.txt", "w");

    #pragma omp parallel num_threads(NUM_PRODUCERS + NUM_CONSUMERS)
    {
        int id = omp_get_thread_num();

        // PRODUCER THREADS
        if (id < NUM_PRODUCERS) {
            FILE *fp = fopen(files[id], "r");
            char line[MAX_LINE];

            while (fgets(line, MAX_LINE, fp)) {
                produce(line);
            }

            fclose(fp);
        }

        // CONSUMER THREADS
        else {
            char line[MAX_LINE];

            while (1) {
                int success = consume(line);

                if (success) {
                    omp_set_lock(&file_lock);
                    fputs(line, output);
                    omp_unset_lock(&file_lock);
                }
            }
        }
    }

    fclose(output);

    omp_destroy_lock(&lock);
    omp_destroy_lock(&file_lock);

    return 0;
}
