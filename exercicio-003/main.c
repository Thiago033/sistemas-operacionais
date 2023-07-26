#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_SIZE 10000 
#define MAX_THREADS 10 

int vetor[MAX_SIZE]; 
int vetor_size; 
int num_threads; 
int soma_global = 0;
pthread_mutex_t mutex; 

void* somarElementos(void* arg) {
    int thread_id = *(int*)arg;
    int inicio = thread_id * (vetor_size / num_threads);
    int fim = inicio + (vetor_size / num_threads);

    if (thread_id == num_threads - 1) {
        fim = vetor_size;
    }

    int soma_local = 0;

    for (int i = inicio; i < fim; i++) {
        soma_local += vetor[i];
    }

    pthread_mutex_lock(&mutex);
    soma_global += soma_local; 
    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);
}

int main() {
    printf("Digite o tamanho do vetor (>= 100): ");
    scanf("%d", &vetor_size);

    printf("Digite o número de threads (>= 2 e <= 10): ");
    scanf("%d", &num_threads);

    if (vetor_size < 100 || num_threads < 2 || num_threads > MAX_THREADS) {
        printf("Tamanho do vetor ou número de threads inválido.\n");
        return 1;
    }

    for (int i = 0; i < vetor_size; i++) {
        vetor[i] = i + 1;
    }

    pthread_mutex_init(&mutex, NULL);

    pthread_t threads[MAX_THREADS];
    int thread_ids[MAX_THREADS];

    for (int i = 0; i < num_threads; i++) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, somarElementos, &thread_ids[i]);
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&mutex);

    printf("Soma dos elementos do vetor: %d\n", soma_global);

    return 0;
}