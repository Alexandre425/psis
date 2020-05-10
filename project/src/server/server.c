#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <SDL2/SDL.h>
#include <signal.h>

#include "server_connection.h"
#include "server.h"
#include "../common/UI_library.h"
#include "../common/pacman.h"
#include "../common/vector.h"
#include "../common/utilities.h"

pthread_mutex_t player_array_lock;

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

Player* player_find_by_id(Game* game, unsigned int player_id)
{
    for (unsigned int i = 0; i < game->n_players; ++i)
    {
        if (game->players[i].player_id == player_id)
        {
            return &game->players[i];
        }
    }
    return NULL;
}

unsigned int player_create(Game* game)
{
    pthread_mutex_lock(&player_array_lock);
    // If no players exist (array uninitialized)
    if (!game->players)
    {
        game->n_players++;
        game->players = malloc_check(sizeof(Player));
    }
    else
    {
        game->n_players++;
        game->players = realloc_check(game->players, game->n_players * sizeof(Player));
    }
    // Default color (black)
    game->players[game->n_players-1].color = 0x000000;
    // Find an unused player_id
    unsigned int player_id = 1;
    while(player_find_by_id(game, player_id))
    {
        // Increment the ID if it is already in use
        player_id++;
    }
    game->players[game->n_players-1].player_id = player_id;

    // Put characters in a random position


    pthread_mutex_unlock(&player_array_lock);

    return player_id;
}

void player_destroy(Game* game, unsigned int player_id)
{
    pthread_mutex_lock(&player_array_lock);

    // Find where the player is stored
    Player* player = player_find_by_id(game, player_id);
    
    // Overwrite the player to be destroyed with the last player
    memcpy(player, &game->players[game->n_players-1], sizeof(Player));

    // Make the player array smaller
    game->n_players--;
    game->players = realloc_check(game->players, game->n_players * sizeof(Player));
    
    pthread_mutex_unlock(&player_array_lock);
}


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
    // Initialized to zero due to the nature of malloc_check()
    Game* game = malloc_check(sizeof(Game));

    // Initialize the mutex
    if (pthread_mutex_init(&player_array_lock, NULL))
    {
        perror("ERROR - Mutex init failed");
        exit(EXIT_FAILURE);
    }

    // Read the board
    read_board(game, "board-drawer/board.txt");

    // Calculate maximum number of clients

    // Etc

    // Thread for handling incoming client connections
    pthread_t connect_to_clients_thread;
    pthread_create(&connect_to_clients_thread, NULL, connect_to_clients, (void*)game);

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

    pthread_kill(connect_to_clients_thread, SIGUSR1);    
    pthread_join(connect_to_clients_thread, NULL);

    return 0;
}