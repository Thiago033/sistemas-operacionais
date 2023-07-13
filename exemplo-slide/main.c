#include <stdio.h>
#include <unistd.h> //Para utilizar sleep
#include <pthread.h>
void *Funcao(void *voidargumento){
    printf ("Sou a thread %u e pertenço ao processo %d\n",(unsigned int)pthread_self(), getpid());
    sleep(30);
    printf("Finalizando thread\n");
    fflush(stdout);
}
int main(void){
    pthread_t id;
    pthread_create(&id, NULL, Funcao, NULL);
    sleep(1);
    pthread_exit(NULL); //o que ocorre se retirar essa linha? nao aconteceu nada
return (0);
}