#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#define NUM_THREADS 5

int saldo = 1000;

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

void *AtualizaSaldo(void *threadid) {
    int meu_saldo = saldo;
    int novo_saldo = meu_saldo + (long)threadid*100;

    pthread_mutex_lock(&m);

    printf("Depositando %ld --> Novo saldo = %d\n", (long)threadid*100, novo_saldo);

    saldo = novo_saldo;

    pthread_mutex_unlock(&m);
}

int main (int argc, char *argv[]){
    pthread_t threads[NUM_THREADS];
    int rc;
    long t;

    pthread_mutex_init(&m, NULL);

    printf("Saldo inicial = %d\n", saldo);

    for(t=0; t<NUM_THREADS; t++){
        rc = pthread_create(&threads[t], NULL, AtualizaSaldo, (void *)t);
        if (rc) exit(-1);
    }

    for(t=0; t<NUM_THREADS; t++){
        pthread_join(threads[t], NULL);
    }

    printf("Saldo final = %d\n", saldo);
}