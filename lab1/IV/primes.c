#include<stdio.h>
#include<stdlib.h>
#include<time.h>

int main (void)
{
    int num;
    srand(time(NULL));
    fprintf(stdout, "Found the following multiples of 2, 3, 5 and 7:\n");
    for (int i = 0; i < 10; ++i)
    {
        num = rand();
        if ((num % 2 == 0) || (num % 3 == 0) || (num % 5 == 0) || (num % 7 == 0))
            fprintf(stdout, "%d, ", num);
    }
    fprintf(stdout, "\n");

    return 0;
}