#include <stdio.h>
#include <stdlib.h>
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

  mult(num_vec, MAXNUM, 2);
  mult(num_vec, MAXNUM, 3);
  mult(num_vec, MAXNUM, 5);
  mult(num_vec, MAXNUM, 7);

  exit(0);
}
