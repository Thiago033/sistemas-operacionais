#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

struct arguments {
  int a, b;
};

void* sum (void* void_args) {
  struct arguments* args = (struct arguments *) void_args;
  float* result = malloc(sizeof(float));
  *result = args->a + args->b;
  return (void*) result;
}

int main () {
  struct arguments args;
  args.a = 1;
  args.b = 2;
  float* result;
  pthread_t t1;
  pthread_create(&t1, NULL, sum, &args);
  pthread_join(t1, (void**) &result);
  printf("Resultado: %.2f\n", *result);
  return 0;
}
