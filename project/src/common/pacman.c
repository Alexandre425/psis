#include <stdlib.h>
#include <stdio.h>

#include "pacman.h"
#include "utilities.h"


// The game board
typedef struct _Board
{
    Vector* board_size;
    unsigned int** board;
} Board;

Board* board_create(unsigned int x, unsigned int y)
{
    Board* board = malloc_check(sizeof(Board));
    board->board_size = vec_create(x, y);
    // Allocate the columns
    board->board = malloc_check(x * sizeof(unsigned int*));
    // Allocate the lines
    for (unsigned int i = 0; i < x; ++i)
        board->board[i] = malloc_check(y * sizeof(unsigned int*));

    return board;
}

void board_destroy(Board* board)
{
    if (!board)
    {
        puts("ERROR - Attempted to destroy a NULL board");
        exit(EXIT_FAILURE);
    }
    // Free the board
    for (unsigned int i = 0; i < vec_get_x(board->board_size); ++i)
        free(board->board[i]);
    free(board->board);
    // Free the size vector
    vec_destroy(board->board_size);
    // Free the board struct
    free(board);
}

void board_set_tile(Board* board, unsigned int x, unsigned int y, unsigned int value)
{
    board->board[x][y] = value; 
}


unsigned int board_get_size_x(Board* board)
{
    return vec_get_x(board->board_size);
}

unsigned int board_get_size_y(Board* board)
{
    return vec_get_y(board->board_size);
}

unsigned int board_get_tile(Board* board, unsigned int x, unsigned int y)
{
    return board->board[x][y];
}

unsigned int board_tile_type_to_player_id(unsigned int tile_type)
{
    // Handles both pacman and monster due to integer division
    return (tile_type - TILE_LAST) / (int)2;
}

unsigned int board_player_id_to_tile_type(unsigned int player_id, int is_pacman)
{
    // player_ids are stored after the basic tiles, the last of which is tile_last
    // for example, player 7's pacman would be represented as tile_last + 15
    // their monster would be tile_last + 14
    // this is unique to their player_id
    return TILE_LAST + (player_id*2) + is_pacman;
}

// A fruit's type and position
typedef struct _Fruit
{
    int fruit_type;
    Vector* pos;
} Fruit;