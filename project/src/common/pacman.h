#pragma once

#include "vector.h"

// Defines structures pertaining to the game, that both the client and the server should recognize

enum FruitType {fruit_lemon, fruit_cherry};

enum TileType {tile_empty, tile_brick};

// The game board
typedef struct _Board Board;

// Sets the game board's size and allocates the necessary space
Board* board_create(int x, int y);
// Frees all the memory used by the board
void board_destroy(Board* board);

// Set's the tile at (x,y) to "value"
void board_set_tile(Board* board, int x, int y, unsigned int value);

// Returns the game board's size in columns
unsigned int board_get_size_x(Board* board);
// Returns the game board's size in lines   
unsigned int board_get_size_y(Board* board);
// Returns the TileType of the tile in (x,y)
unsigned int board_get_tile(Board* board, int x, int y);






// A fruit's type and position
typedef struct _Fruit Fruit;