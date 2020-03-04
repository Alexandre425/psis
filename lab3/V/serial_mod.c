#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAXNUM 10000

void mult(int vec[], int len, int n){
  int i;
  for(i=0; i< len; i++){
    if(vec[i]%n == 0)
      printf("mult %d %d\n", n, vec[i]);
  }
}

int main(){
  int *num_vec;
  int i;

  num_vec= malloc(sizeof(int)*MAXNUM);
  if(num_vec==NULL){
    exit(-1);
  }

  for(i=0; i< MAXNUM; i++){
    num_vec[i]= random();
  }

  const unsigned int prime_array[4] = {2, 3, 5, 7};
  for (int i = 0; i < 4; ++i)
  {
    if (fork() == 0)
    {
      mult(num_vec, MAXNUM, prime_array[i]);
      break;
    }
  }

  // Wait for completion
  for (int i = 0; i < 4; ++i)
    wait(NULL);

  exit(0);
}
