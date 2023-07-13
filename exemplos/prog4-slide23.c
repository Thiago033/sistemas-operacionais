#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

void *Funcao(void *argumento){
        printf ("Sou a thread %ld e pertencao ao processo %d\n",pthread_self(), getpid());
        sleep(60);
}
int main(void){
        pthread_t id;
        pthread_create(&id, NULL, Funcao, NULL);
        pthread_join(id, NULL);//Mesma funcionalidade de pthread_exit(NULL);?
}
