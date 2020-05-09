#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "message.h"

void message_misaligned(void)
{
    puts("ERROR - Message misalignment detected");
    exit(EXIT_FAILURE);
}


typedef struct __attribute__((__packed__)) _packed_color 
{
    uint32_t color;
} packed_Color;

packed_Color* message_packed_color_create(void)
{
    return malloc_check(sizeof(packed_Color));
}

void message_packed_color_destroy(packed_Color* p_color)
{
    free(p_color);
}

size_t message_pack_color(Color* color, packed_Color* p_color)
{
    p_color->color = *color;
    return sizeof(packed_Color);
}


