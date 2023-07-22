#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#define ARRAY_SIZE 10
#define NUM_THREADS 1

int array[ARRAY_SIZE];
int sum = 0;

pthread_t thread_array[NUM_THREADS];


int numsPerThread = ARRAY_SIZE / NUM_THREADS;


void* ArraySum(void* arg) {

    int thread_id = *(int)

    for (int j = 0; j < numsPerThread; j++) {
        sum = array[j];
    }


    return NULL; 
}

// 1 2 3 4 5   |   6 7 8 9 10


int main(int argc, char const *argv[]) {
    
    // Create array
    for (int i = 0; i < ARRAY_SIZE; i++) {
        array[i] = i +1;
    }


    // Create thread array
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&thread_array[i], NULL, ArraySum, i+1);
    }


    printf("array sum: %d", sum);


    











    return 0;
}
