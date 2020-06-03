#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<pthread.h>
#include<fcntl.h>
#include<assert.h>
#include<signal.h>


#define FIFO_PATH "/tmp/server.fifo"

void cleanup_and_quit(int signal)
{
    unlink(FIFO_PATH);
    exit(0);
}



void* client_handler(void* _client_pid)
{
    int client_pid = *(int*)_client_pid;
}

int main (void) 
{
    signal(SIGINT, cleanup_and_quit);

    // Make a new fifo (main FIFO for receiving PIDs)
    unlink(FIFO_PATH);
    assert(mkfifo(FIFO_PATH, 0666) == 0);
    puts("FIFO created, waiting for clients");
    int fd = open(FIFO_PATH, O_RDONLY); // Open the FIFO in read only mode

    // Receive PIDs and open new threads to handle them
    int client_pid;
    while (1)
    {
        read(fd, &client_pid, sizeof(int));
        fprintf(stdin, "PID %d connected\n", client_pid);
        pthread_t thread;
        pthread_create(&thread, NULL, client_handler, &client_pid);
    }
    


    return 0;
}