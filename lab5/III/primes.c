#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <assert.h>
#include <pthread.h>

void* readPrimes(void* fd)
{
    int read_fd = *(int*)fd;
    int prime;

    while (read(read_fd, &prime, sizeof(int)) != 0)
    {
        printf("Prime: %d\n", prime);
    }

    pthread_exit(0);
}

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

void receiveAndTest(int read_file, int write_file)
{
    int received_num = 0;
    unsigned int prime_count = 0;

    // Reading while pipe is open
    while (1)
    {
        read(read_file, &received_num, sizeof(int));
        if (received_num == -1)
            break;
        if (isPrime(received_num))
        {
            // Send the number through the pipe
            write(write_file, &received_num, sizeof(int));
            prime_count++;
        }
    }
    close(write_file);
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

    int read_fd[2], write_fd[2];

    // Setting up the prime number reading
    assert(pipe(read_fd) == 0);
    pthread_t handle;
    pthread_create(&handle, NULL, readPrimes, &read_fd[0]);

    // Forking children
    assert(pipe(write_fd) == 0);
    for (int i = 0; i < n_children; ++i)
    {
        if (fork() == 0)
        {
            receiveAndTest(write_fd[0], read_fd[1]);
            exit(0);
        }
    }

    // Closing the unnecessary pipes
    close(write_fd[0]);
    close(read_fd[1]);

    // RNG and sending
    srand(getpid() * n_num + n_children);
    for (int i = 0; i < n_num; ++i)
    {
        int rand_num = rand() % 100000;
        write(write_fd[1], &rand_num, sizeof(int));
    }

    // Close the sending pipe to trigger the exit from the children
    for (int i = 0; i < n_children; ++i)
    {
        int signal = -1;
        write(write_fd[1], &signal, sizeof(int));
        wait(NULL);
    }
    // Close the prime reading pipe after all children exit
    close(read_fd[0]);
    // Waiting for the reading thread
    pthread_join(handle, NULL);


    return 0;
}

