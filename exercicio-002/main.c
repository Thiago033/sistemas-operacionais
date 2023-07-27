#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#define ARRAY_SIZE 10
#define NUM_THREADS 3

int array[ARRAY_SIZE];
int sum = 0;

pthread_t thread_array[NUM_THREADS];
pthread_t thread_index[NUM_THREADS];

pthread_mutex_t mutex;


void* ArraySum(void* arg) {
    int thread_index = *(int*)arg;

    int start = thread_index * (ARRAY_SIZE / NUM_THREADS);
    int end = (thread_index + 1) * (ARRAY_SIZE / NUM_THREADS);

    if (thread_index == (NUM_THREADS - 1)) {
        end = ARRAY_SIZE;
    }

    for (int i = start; i < end; i++) {
        pthread_mutex_lock(&mutex);
        sum += array[i];
        pthread_mutex_unlock(&mutex);
    }
    
    return NULL; 
}

int main(int argc, char const *argv[]) {
    pthread_mutex_init(&mutex, NULL);

    for (int i = 0; i < ARRAY_SIZE; i++) {
        array[i] = i + 1;
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        thread_index[i] = i;
        pthread_create(&thread_array[i], NULL, ArraySum, &thread_index[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(thread_array[i], NULL);
    }

    printf("array sum: %d", sum);

    pthread_mutex_destroy(&mutex);

    return 0;
}
