#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>
#include <sys/syscall.h>
int vet[10];
void * Funcao (void * argumento){
        int myid = *(int *)argumento;
        printf("%d: O numero e %d\n", myid, vet[myid]);
	//fflush(stdout);
        pthread_exit(NULL);
}
int main(void){
        pthread_t id[10];
        int i=0;
        FILE *arq;
        arq=fopen("matricula.txt","r");
	if(arq==NULL){
		printf("Arquivo Inexistente"); exit(0);
	}
        while(!feof(arq)){
                fscanf(arq,"%d",&vet[i]);
                i++;
        }
        for (i=0;i<10;i++)
                pthread_create (&id[i], NULL, Funcao, (void *) &i);
        for(i=0;i<10;i++)
                pthread_join(id[i], NULL);
	return 0;
}
