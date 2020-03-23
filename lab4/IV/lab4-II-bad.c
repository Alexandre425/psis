#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <pthread.h>
#include <unistd.h>

void *thread_func (void * arg){
  char c;
  int n = 0;
  while (c != 'k'){
    printf("Thread_id %x - waiting for char \n", pthread_self());
    fscanf(stdin, "%c", &c);
    sleep(1);
    n++;
  }
  printf("exiting %x - read %d chars \n", pthread_self(), n);
  return NULL;
}
int main(int argc, char * argv[]){
  int n_threads;
  pthread_t thread_id;
  if(argc ==1 || sscanf(argv[1], "%d", & n_threads) != 1)
    exit(-1);

  printf("Creating %d Threads\n", n_threads);
  for (int n = 0; n < n_threads; n++)
    pthread_create(&thread_id, NULL, thread_func, NULL);

  thread_func(NULL);

  exit(0);


}
