#include<stdio.h>
#include<stdlib.h>

int main (void)
{
    int buffer[100] = {0};
    int i = 0;
    while(1)
    {
        fprintf(stdout, "Read position\t%d:\t%d \n", i++, buffer[i]);
    }

    return 0;
}