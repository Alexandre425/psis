#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main (void)
{
    puts("Insert a string");
    char string[100] = {0};
    fgets(string, 100, stdin);

    // If in a child process
    if (fork() == 0)
    {
        puts("Child process prints:");
        puts(string);
    }

    return 0;
}