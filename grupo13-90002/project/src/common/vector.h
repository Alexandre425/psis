#pragma once

// Simple integer vector functions

typedef struct _Vec Vector;

// Creates a new vector
Vector* vec_create (int x, int y);
// Destroys a vector
void vec_destroy (Vector* vec);

// Sets both vector variables
void vec_set (Vector* vec, int x, int y);
// Sets the vector's x variable
void vec_set_x (Vector* vec, int x);
// Sets the vector's y variable
void vec_set_y (Vector* vec, int y);

// Gets the vector's x variable
int vec_get_x (Vector* vec);
// Gets the vector's y variable
int vec_get_y (Vector* vec);
// Puts both of the vector's variables in x and y
void vec_get (Vector* vec, int* x, int* y);
