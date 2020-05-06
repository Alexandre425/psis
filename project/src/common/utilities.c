#include "utilities.h"

void* malloc_check(size_t size)
{
    void* ptr = calloc(1, size);
    if (ptr == NULL)
    {
        perror("ERROR - Could not allocate memory");
        exit(EXIT_FAILURE);
    }
    return ptr;
}