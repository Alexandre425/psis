#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <pthread.h>
#include <unistd.h>

void *thread_func (void * arg){
  int n = 0;
  while (n<10){
    printf("Thread_id %x - %d \n", pthread_self(), n);
    sleep(1);
  }
  printf("exiting %x \n", pthread_self() );
  return NULL;
}
int main(int argc, char * argv[]){
  int n_threads;
  pthread_t thread_id;
  int n;
  if(argc ==1 || sscanf(argv[1], "%d", & n_threads) != 1)
    exit(-1);

  printf("Creating %d Threads\n", n_threads);
  for (n= 0; n < n_threads; n++)
    pthread_create(&thread_id, NULL, thread_func, NULL);

  getchar();

  exit(0);


}
