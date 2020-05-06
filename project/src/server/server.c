#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <SDL2/SDL.h>

#include "server_connection.h"
#include "../common/UI_library.h"
#include "../common/pacman.h"
#include "../common/vector.h"
#include "../common/utilities.h"

typedef struct _Player
{
    unsigned int player_id;
    Color color;
    bool powered_up;

} Player;

typedef struct _Game
{
    Board board;
    unsigned int n_players;
    Player* players;
    Fruit* fruits;
} Game;

// Reads the board file and initializes the board in the game struct
void read_board(Game* game, char* path)
{
    FILE* board_file = fopen(path, "r");
    if (board_file == NULL)
    {
        perror("ERROR - Could not open board file");
        exit(EXIT_FAILURE);
    }
    // Read the board size
    fscanf(board_file, "%d %d", &game->board.board_size.x, &game->board.board_size.y);
    // Allocate collumns
    game->board.board = malloc_check(sizeof(int*) * game->board.board_size.x);
    // Allocate lines
    for (int i = 0; i < game->board.board_size.y; ++i)
    {
        game->board.board[i] = malloc_check(sizeof(int) * game->board.board_size.y);
    }

    // Read the board line by line
    int tile = 0; 
    for (int i = 0; i < game->board.board_size.y; ++i)
    {
        // Catch the \n
        fgetc(board_file);
        for (int j = 0; j < game->board.board_size.x; ++j)
        {
            tile = fgetc(board_file);
            switch (tile)
            {
                case ' ':
                    game->board.board[j][i] = tile_empty;
                break;

                case 'B':
                    game->board.board[j][i] = tile_brick;
                break;
                
                default:
                    puts("ERROR - Bad character in tile map");
                    exit(EXIT_FAILURE);
                break;
            }
        }
    }
    puts("Tile map read!");
    fclose(board_file);
}

int main (void)
{
    // Initialize the game structure
    Game* game = malloc_check(sizeof(Game));

    // Read the board
    read_board(game, "board-drawer/board.txt");

    // Calculate maximum number of clients

    // Etc

    // Thread for handling incoming client connections
    pthread_t connect_to_clients_thread;
    pthread_create(&connect_to_clients_thread, NULL, connect_to_clients, NULL);

    pthread_join(connect_to_clients_thread, NULL);

    return 0;
}