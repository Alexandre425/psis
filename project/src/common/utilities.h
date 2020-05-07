#pragma once

// Random useful functions, defs and structs

typedef u_int32_t Color; // Color stored in 0xRRGGBB format

// Allocates, zeroes and checks SIZE bytes of memory
void* malloc_check(size_t SIZE);

// Ensures all the data is sent
// Returns 1 on success, -1 on error
int send_all (int socket, void* buffer, size_t size);

// Ensures all the data is received
// Returns 1 on success, 0 on close, -1 on error
int recv_all (int socket, void* buffer, size_t size);
