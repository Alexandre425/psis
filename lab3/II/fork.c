#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>

int main (void)
{
    u_int8_t child = 0;
    const unsigned int proc_num = 10;
    for (int i = 0; i < proc_num; ++i)
    {
        if (fork() == 0)    // Process is a child
        {
            child = 1;
            break;
        }
    }

    if (child)
    {
        pid_t pid = getpid();
        srand(pid);
        unsigned int sleep_time = rand() % 10; 
        sleep(sleep_time);
        printf("Process %d exits after %d seconds\n", pid, sleep_time);
    }
    return 0;
}