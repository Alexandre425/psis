#pragma once

#include "vector.h"

// Defines structures pertaining to the game, that both the client and the server should recognize

enum FruitType {FRUIT_LEMON, FRUIT_CHERRY};

enum TileType {TILE_EMPTY, TILE_BRICK, TILE_LAST};

// The game board
typedef struct _Board Board;

// Sets the game board's size and allocates the necessary space
Board* board_create(unsigned int x, unsigned int y);
// Frees all the memory used by the board
void board_destroy(Board* board);

// Set's the tile at (x,y) to "value"
void board_set_tile(Board* board, unsigned int x, unsigned int y, unsigned int tile_type);

// Returns the game board's size in columns
unsigned int board_get_size_x(Board* board);
// Returns the game board's size in lines   
unsigned int board_get_size_y(Board* board);
// Returns the TileType of the tile in (x,y)
unsigned int board_get_tile(Board* board, unsigned int x, unsigned int y);

// Translates board tile type to true player id
unsigned int board_tile_type_to_player_id(unsigned int tile_type);

// Translates the player id into a tile type to be stored on the board
// is_pacman should be 1 if a pacman is to be stored, or 0 if a monster
unsigned int board_player_id_to_tile_type(unsigned int player_id, int is_pacman);







// A fruit's type and position
typedef struct _Fruit Fruit;