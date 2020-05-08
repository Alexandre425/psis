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

Board* board_create(int x, int y)
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

void board_set_tile(Board* board, int x, int y, unsigned int value)
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

unsigned int board_get_tile(Board* board, int x, int y)
{
    return board->board[x][y];
}


// A fruit's type and position
typedef struct _Fruit
{
    int fruit_type;
    Vector* pos;
} Fruit;