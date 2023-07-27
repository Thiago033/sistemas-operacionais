#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_PHILOSOPHERS 5
#define NUM_FOOD 5

pthread_t philosophers[NUM_PHILOSOPHERS];

pthread_mutex_t hashis[NUM_PHILOSOPHERS];

int food = NUM_FOOD;

int thread_id[NUM_PHILOSOPHERS];


void *philosophers_func( void *args) {
    int thread_id = *((int *)args);


}

int main() {

    // each hashi receive a mutex
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_mutex_init(&hashis[i], NULL);
    }

    // each philosopher is a different thread
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        thread_id[i] = i;
        pthread_create(&philosophers[i], NULL, philosophers_func, &thread_id[i]);

    }

    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_join(philosophers[i], NULL);
    }

    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_mutex_destroy(&hashis[i]);
    }
}