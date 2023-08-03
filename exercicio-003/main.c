#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include<unistd.h>
#include<time.h>

#define NUM_PHILOSOPHERS 5

pthread_t philosophers[NUM_PHILOSOPHERS];

pthread_mutex_t hashis[NUM_PHILOSOPHERS];

int philosopher_id[NUM_PHILOSOPHERS];

void think(int philosofer_index) {
    printf("Thinking: %d\n", philosofer_index);
    sleep((rand() % 3) + 1);
}

void eat(int philosofer_index) {
    printf("Eating: %d\n", philosofer_index);
    sleep(rand() % 3 + 1);

}

void take_hashi(int philosofer_index) {
    printf("taking hashis: %d\n", philosofer_index);

    int right_hashi = philosofer_index;
    int left_hashi = (philosofer_index + 1) % NUM_PHILOSOPHERS;

    //lock hashi left and right
    pthread_mutex_lock(&hashis[right_hashi]);
    pthread_mutex_lock(&hashis[left_hashi]);
}

void drop_hashi(int philosofer_index) {
    printf("droping hashis: %d\n", philosofer_index);

    int right_hashi = philosofer_index;
    int left_hashi = (philosofer_index + 1) % NUM_PHILOSOPHERS;

    //unlock hashi left and right
    pthread_mutex_unlock(&hashis[right_hashi]);
    pthread_mutex_unlock(&hashis[left_hashi]);
}

void *table( void *args) {
    int philosopher_id = *((int *)args);

    think(philosopher_id);
    take_hashi(philosopher_id);
    eat(philosopher_id);
    drop_hashi(philosopher_id);

    printf("thread finished: %d\n", philosopher_id);
}

int main() {
    srand(time(NULL));

    // each hashi receive a mutex
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_mutex_init(&hashis[i], NULL);
    }

    // each philosopher is a different thread
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        philosopher_id[i] = i;
        pthread_create(&philosophers[i], NULL, table, &philosopher_id[i]);

    }

    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_join(philosophers[i], NULL);
    }

    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_mutex_destroy(&hashis[i]);
    }
}