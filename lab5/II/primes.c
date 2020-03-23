#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <assert.h>

int isPrime(int num)
{
    // 1, 2 and 3 are primes
    if (num < 4)
        return 1;

    for (int i = 2; i <= num / 2; ++i)
    {
        // If it's divisable, it is not prime
        if (num % i == 0)
            return 0;
    }

    // If not, it is a prime
    return 1;
}

void receiveAndTest(int fd[2])
{
    // Close the writing pipe
    close(fd[1]);

    int received_num = 0;
    unsigned int prime_count = 0;

    // Reading while pipe is open
    while (read(fd[0], &received_num, sizeof(int)) != 0)
    {
        if (isPrime(received_num))
        {
            printf("Prime: %d\n", received_num);
            prime_count++;
        }
    }
    printf("PID %d exits \t|\t Read %u primes\n", getpid(), prime_count);
}

int main (int argc, char* argv[])
{
    if (argc != 3)
    {
        puts("Wrong number of args");
        exit(-1);
    }

    // Terminal input
    unsigned int n_num, n_children;
    sscanf(argv[1], "%d", &n_num);
    sscanf(argv[2], "%d", &n_children);
    assert(n_num != 0 && n_children != 0);

    int fd[2];
    assert(pipe(fd) == 0);

    // Forking children
    for (int i = 0; i < n_children; ++i)
    {
        if (fork() == 0)
        {
            receiveAndTest(fd);
            exit(0);
        }
    }

    // Closing the reading pipe
    close(fd[0]);

    // RNG and sending
    srand(getpid() * n_num + n_children);
    for (int i = 0; i < n_num; ++i)
    {
        int rand_num = rand() % 10000;
        write(fd[1], &rand_num, sizeof(int));
    }

    // Close the pipe
    close(fd[1]);
    for (int i = 0; i < n_children; ++i)
    {
        wait(NULL);
    }


    return 0;
}

