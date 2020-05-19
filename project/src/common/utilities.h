#pragma once

#include <stdint.h>
#include <time.h>

// Random useful functions, defs and structs

typedef uint32_t Color; // Color stored in 0xRRGGBB format

// Converts a color in the hexadecimal format to the rgb format
void color_hex_to_rgb(Color color, unsigned int* r, unsigned int* g, unsigned int* b);

// Allocates, zeroes and checks SIZE bytes of memory
void* malloc_check(size_t SIZE);
// Reallocates memory at ptr to SIZE bytes and checks
void* realloc_check(void* ptr, size_t SIZE);

// Returns the absolute value of an integer
int abs_int(int value);

// Calculates the difference between two timespec structs
// Returns the result in miliseconds
int time_diff_ms(struct timespec then, struct timespec now);

// Ensures all the data is sent
// Also checks for errors, as
// Returns 1 on success, -1 on error
int send_all (int socket, void* buffer, size_t size);
// Ensures all the data is received
// Returns 1 on success, 0 on close, -1 on error
int recv_all (int socket, void* buffer, size_t size);
