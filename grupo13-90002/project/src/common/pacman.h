#pragma once

#include "vector.h"

// Defines structures pertaining to the game, that both the client and the server should recognize

enum FruitType {FRUIT_LEMON, FRUIT_CHERRY};

enum TileType {TILE_EMPTY, TILE_BRICK, TILE_FRUIT, TILE_INVALID};

// The game board
typedef struct _Board Board;

// Sets the game board's size and allocates the necessary space
Board* board_create(unsigned int x, unsigned int y);
// Frees all the memory used by the board
void board_destroy(Board* board);

// Set's the tile at (x,y) to "value"
void board_set_tile(Board* board, int x, int y, unsigned int tile_type);

// Returns the game board's size in columns
unsigned int board_get_size_x(Board* board);
// Returns the game board's size in lines   
unsigned int board_get_size_y(Board* board);
// Returns the TileType of the tile in (x,y)
// If the coordinates are OOB, then TILE_INVALID is returned
unsigned int board_get_tile(Board* board, int x, int y);

// Translates board tile type to true player id
unsigned int board_tile_type_to_player_id(unsigned int tile_type);
// Translates board tile type to character type
// Returns 1 if pacman, 0 if monster
int board_tile_type_is_pacman(unsigned int tile_type);
// Translates the player id into a tile type to be stored on the board
// is_pacman should be 1 if a pacman is to be stored, or 0 if a monster
unsigned int board_player_id_to_tile_type(unsigned int player_id, int is_pacman);
// Finds an empty space in the board at random
// Puts the resulting coordinates in x, y
// If there is no empty space, x and y become -1
void board_random_empty_space(Board* board, int* x, int* y);
// Checks if the target coordinates are out of bounds
// Returns 1 if OOB, 0 otherwise
int board_is_oob(Board* board, int x, int y);
