#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define VECTOR_SIZE 100
#define NUM_THREADS 5

int vetor[VECTOR_SIZE];
int soma = 0;
pthread_mutex_t mutex;

// Função executada por cada thread
void *sumThread(void *arg) {
    int thread_id = *(int*)arg;
    int elements_per_thread = VECTOR_SIZE / NUM_THREADS;
    int start_index = thread_id * elements_per_thread;
    int end_index = (thread_id == NUM_THREADS - 1) ? VECTOR_SIZE : start_index + elements_per_thread;

    int partial_sum = 0;
    for (int i = start_index; i < end_index; i++) {
        partial_sum += vetor[i];
    }

    pthread_mutex_lock(&mutex);
    soma += partial_sum;
    pthread_mutex_unlock(&mutex);

    return NULL;
}

int main() {
    // Inicializando o vetor com valores sintéticos (1, 2, 3, ...)
    for (int i = 0; i < VECTOR_SIZE; i++) {
        vetor[i] = i + 1;
    }

    // Inicializando o mutex
    pthread_mutex_init(&mutex, NULL);

    // Criando as threads
    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, sumThread, &thread_ids[i]);
    }

    // Aguardando a conclusão das threads
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // Destruindo o mutex
    pthread_mutex_destroy(&mutex);

    // Imprimindo o resultado
    printf("Soma dos elementos do vetor: %d\n", soma);

    return 0;
}
