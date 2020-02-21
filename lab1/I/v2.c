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
    char* concat_str = calloc(len, sizeof(char));
    // Destination pointer
    char* dest = concat_str;

    // For every argument
    for (int i = 0; i < argc; ++i)
    {
        // Size of the block to copy in bytes
        size_t block_size = strlen(argv[i]) * sizeof(char);
        memcpy(dest, argv[i], block_size);
        dest += block_size;
    }
    printf("%s\n", concat_str);

    return 0;
}