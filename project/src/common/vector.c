#include <stdlib.h>
#include <stdio.h>

#include "vector.h"
#include "utilities.h"

typedef struct _Vec
{
    int x;
    int y;
} Vector;

Vector* vec_create (int x, int y)
{
    Vector* vec = malloc_check(sizeof(Vector));
    vec->x = x;
    vec->y = y;
    return vec;
}

void vec_destroy (Vector* vec)
{
    free(vec);
}


void vec_set (Vector* vec, int x, int y)
{
    if (!vec)
    {
        puts("ERROR - Attempted to set a null vector");
        exit(EXIT_FAILURE);
    }
    vec->x = x;
    vec->y = y;
}

void vec_set_x (Vector* vec, int x)
{
    if (vec == NULL)
    {
        puts("ERROR - Attempted to set a null vector");
        exit(EXIT_FAILURE);
    }
    vec->x = x;
}

void vec_set_y (Vector* vec, int y)
{
    if (vec == NULL)
    {
        puts("ERROR - Attempted to set a null vector");
        exit(EXIT_FAILURE);
    }
    vec->y = y;
}

int vec_get_x (Vector* vec)
{
    return vec->x;
}
int vec_get_y (Vector* vec)
{
    return vec->y;
}
void vec_get (Vector* vec, int* x, int* y)
{
    *x = vec->x;
    *y = vec->y;
}