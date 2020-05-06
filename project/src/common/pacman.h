#pragma once

#include "vector.h"

// Defines structures pertaining to the game, that both the client and the server should recognize

typedef uint32_t Color; // Color stored in 0xRRGGBB format

enum FruitType {fruit_lemon, fruit_cherry};

enum TileType {tile_empty, tile_brick};

// The game board
typedef struct _Board
{
    Vector board_size;
    unsigned int** board;
} Board;

// A fruit's type and position
typedef struct _Fruit
{
    int fruit_type;
    Vector pos;
} Fruit;