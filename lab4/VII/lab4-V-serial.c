#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define MAXNUM 10000

typedef struct mult_t
{
  int* vec;
  int len;
  int multiple;
}mult_t;

void* mult(void* _arg){
  mult_t* arg = (mult_t*)_arg;
  int count = 0;
  for(count = 0; count < arg->len; ++count){
    if(arg->vec[count] % arg->multiple == 0)
      printf("mult %d %d\n", arg->multiple, arg->vec[count]);
  }
  void* ret_ptr = malloc(sizeof(void*));
  *(int*)ret_ptr = count;
  pthread_exit(ret_ptr);
}

int main(){
  int *num_vec;
  int i;

  num_vec = malloc(sizeof(int)*MAXNUM);
  if(num_vec==NULL){
    exit(-1);
  }

  for(i=0; i< MAXNUM; i++){
    num_vec[i]= random();
  }

  mult_t mult_arg[4];
  mult_arg[0].vec = num_vec;
  mult_arg[0].len = MAXNUM;
  for (int i = 1; i < 4; i++)
  {
    mult_arg[i] = mult_arg[0];
  }
  mult_arg[0].multiple = 2;
  mult_arg[1].multiple = 3;
  mult_arg[2].multiple = 5;
  mult_arg[3].multiple = 7;

  pthread_t thread[4];
  for (int i = 0; i < 4; ++i)
  {
    pthread_create(&thread[i], NULL, mult, &mult_arg[i]);
  }

  void* count_ptr = NULL;
  for (int i = 0; i < 4; ++i)
  {
    pthread_join(thread[i], &count_ptr);
    fprintf(stdout, "Thread %x exits after %d counts\n", (unsigned int)thread[i], *(int*)count_ptr);
  }

  exit(0);
}
