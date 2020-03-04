#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>

const unsigned int proc_num = 10;

int main (void)
{
    u_int8_t child = 0;
    // Parent creates the 10 child processes
    for (int i = 0; i < proc_num && !child; ++i)
    {
        pid_t pid = fork();
        assert(pid != -1);
        if (pid == 0)
            child = 1;
    }

    // Loop waits and forks for the parent
    while(!child)
    {
        int status;
        pid_t pid = wait(&status);
        assert(pid != -1);
        printf("Process %d exits after %d seconds\n", pid, WEXITSTATUS(status));
        if (fork() == 0)
        {
            child = 1;
            break;
        }
    }

    // Timer for the child
    if (child)
    {
        pid_t pid = getpid();
        srand(pid);
        unsigned int sleep_time = rand() % 10 + 1; 
        sleep(sleep_time);
        exit(sleep_time);
    }

    return 0;
}