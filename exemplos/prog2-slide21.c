#include <stdio.h>
#include <unistd.h> //Para utilizar sleep
#include <pthread.h>
void *Funcao(void *argumento){
        printf ("Sou a thread %u e pertencao ao processo %d\n",(unsigned int)pthread_self(), getpid());
	sleep(30);
 	printf("Finalizando thread\n");
        fflush(stdout);
}
int main(void){
        pthread_t id;
        pthread_create(&id, NULL, Funcao, NULL);
        sleep(1);
	pthread_exit(NULL);  //o que ocorre se retirar essa linha?
	return (0);
}
