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
    Board* board;
    unsigned int max_players;
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
    unsigned int x, y;
    fscanf(board_file, "%u %u", &x, &y);
    game->board = board_create(x, y);

    // Read the board line by line
    int tile = 0; 
    for (unsigned int i = 0; i < y; ++i)
    {
        // Catch the \n
        fgetc(board_file);
        for (unsigned int j = 0; j < x; ++j)
        {
            tile = fgetc(board_file);
            switch (tile)
            {
                case ' ':
                    board_set_tile(game->board, j, i, tile_empty);
                break;

                case 'B':
                    board_set_tile(game->board, j, i, tile_brick);
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

// Draws the bricks
void draw_bricks(Game* game)
{
    for (unsigned int i = 0; i < board_get_size_x(game->board); ++i)
    {
        for (unsigned int j = 0; j < board_get_size_y(game->board); ++j)
        {
            if (board_get_tile(game->board, i, j) == tile_brick)
                paint_brick(i, j);
        }
    }
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

    // Create the SDL window
    create_board_window(board_get_size_x(game->board), board_get_size_y(game->board));

    // Draw the board, just the tiles
    draw_bricks(game);
    // Main poll and draw loop
    SDL_Event event;
    bool quit = false;
    while (!quit){
		while (SDL_PollEvent(&event)) {
            switch (event.type)
            {
            case SDL_QUIT:          // Quit the program when the window is closed
                close_board_windows();
                quit = true;
                break;
            
            default:
                break;
            }
		}

        // Clear the board to render over
        clear_board(board_get_size_x(game->board), board_get_size_y(game->board));
        draw_bricks(game);
        render_board();
	}

    pthread_join(connect_to_clients_thread, NULL);

    return 0;
}