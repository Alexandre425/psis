#include<string.h>
#include<stdio.h>
#include<stdlib.h>

unsigned int strlen2(char* str)
{
    unsigned int len = 0;
    while (str[len] != 0)
        len++;
    return len;
}

int main(int argc, char** argv)
{
    int len = 0;
    for (int i = 0; i < argc; ++i)
    {
        len += strlen2(argv[i]);
    }
    char* concat_str = calloc(len + 1, sizeof(char));
    // Destination pointer
    char* dest = concat_str;

    // For every argument
    for (int i = 0; i < argc; ++i)
    {
        // Bytes to copy
        size_t block_size = strlen2(argv[i]) * sizeof(char);
        memcpy(dest, argv[i], block_size);
        dest += block_size;
    }
    printf("%s\n", concat_str);

    return 0;
}