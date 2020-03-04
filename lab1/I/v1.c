#include<string.h>
#include<stdio.h>
#include<stdlib.h>

int main(int argc, char** argv)
{
    int len = 0;
    for (int i = 0; i < argc; ++i)
    {
        len += strlen(argv[i]);
    }
    char* concat_str = calloc(len + 1, sizeof(char));
    for (int i = 0; i < argc; ++i)
    {
        strcat(concat_str, argv[i]);
    }
    printf("%s\n", concat_str);

    return 0;
}