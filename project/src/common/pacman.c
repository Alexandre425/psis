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
    if (board_is_oob(board, x, y))
        return TILE_INVALID;
    else
        return board->board[x][y];
}

unsigned int board_tile_type_to_player_id(unsigned int tile_type)
{
    // Handles both pacman and monster due to how integer division works
    return (tile_type - TILE_INVALID) / (int)2;
}

int board_tile_type_is_pacman(unsigned int tile_type)
{
    return (tile_type - TILE_INVALID) % (int)2;
}

unsigned int board_player_id_to_tile_type(unsigned int player_id, int is_pacman)
{
    // player_ids are stored after the basic tiles, the last of which is tile_last
    // for example, player 7's pacman would be represented as tile_last + 15
    // their monster would be tile_last + 14
    // this is unique to their player_id
    return TILE_INVALID + (player_id*2) + is_pacman;
}

static int board_empty_space_exists(Board* board)
{
    for (unsigned int i = 0; i < vec_get_x(board->board_size); ++i)
        for (unsigned int j = 0; j < vec_get_y(board->board_size); ++j)
            if (board->board[i][j] == TILE_EMPTY)
                return 1;   // 3rd level nesting: -10 points
    return 0;
}

void board_random_empty_space(Board* board, int* x, int* y)
{
    if (!board_empty_space_exists(board))   // This prevents the mythical infinite loop
    {
        *x = -1; *y = -1;
        return;
    }
    do
    {
        *x = rand() % vec_get_x(board->board_size); // It aint efficient
        *y = rand() % vec_get_y(board->board_size); //  but it works
    }
    while (board->board[*x][*y] != TILE_EMPTY);
}

int board_is_oob(Board* board, int x, int y)
{
    int size_x = board_get_size_x(board), size_y = board_get_size_y(board);
    if (x >= 0 && x < size_x && y >= 0 && y < size_y)
        return 0;
    else
        return 1;
}

