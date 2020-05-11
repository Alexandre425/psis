#include <stdlib.h>
#include <stdio.h>
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
    Vector* pacman_pos;
    Vector* monster_pos;
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
    Player* new_player = &game->players[game->n_players-1];
    // Default color (black)
    new_player->color = 0x000000;
    // Find an unused player_id
    unsigned int player_id = 1;
    while(player_find_by_id(game, player_id))
    {
        // Increment the ID if it is already in use
        player_id++;
    }
    new_player->player_id = player_id;

    // Put characters in a random position
    unsigned int max_x = board_get_size_x(game->board);
    unsigned int max_y = board_get_size_y(game->board);
    unsigned int x, y;
    do
    {
        x = rand() % max_x;
        y = rand() % max_y;
    }
    while(board_get_tile(game->board, x, y) != tile_empty);
    new_player->pacman_pos = vec_create(x, y);
    board_set_tile(game->board, x, y, board_player_id_to_tile_type(player_id, 1));
    do
    {
        x = rand() % max_x;
        y = rand() % max_y;
    }
    while(board_get_tile(game->board, x, y) != tile_empty);
    new_player->monster_pos = vec_create(x, y);
    board_set_tile(game->board, x, y, board_player_id_to_tile_type(player_id, 0));

    pthread_mutex_unlock(&player_array_lock);

    return player_id;
}

void player_destroy(Game* game, unsigned int player_id)
{
    pthread_mutex_lock(&player_array_lock);

    // Find where the player is stored
    Player* player = player_find_by_id(game, player_id);
    
    // Clear the spaces on the board occupied by the pacman and monster
    board_set_tile(game->board, vec_get_x(player->pacman_pos), vec_get_y(player->pacman_pos), tile_empty);
    board_set_tile(game->board, vec_get_x(player->monster_pos), vec_get_y(player->monster_pos), tile_empty);

    // Free the position vectors
    free(player->pacman_pos);
    free(player->monster_pos);

    // Overwrite the player to be destroyed with the last player
    memcpy(player, &game->players[game->n_players-1], sizeof(Player));

    game->n_players--;
    // Not strictly necessary, but prevents a bug with player_create where
    // the second client joining an empty server gets assigned player_id 2
    // instead of 1 (as the array still contains the data after the realloc)
    if (game->n_players == 0)
    {
        free(game->players);
        game->players = NULL;
    }
    
    pthread_mutex_unlock(&player_array_lock);
}

void player_set_color(Player* player, Color color)
{
    player->color = color;
}


// Reads the board file and initializes the board in the game struct
// Returns the number of empty spaces on the board
static unsigned int read_board(Game* game, char* path)
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
    unsigned int n_empty = 0;
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
                    n_empty++;
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

    return n_empty;
}

// Draws the bricks
static void draw_bricks(Game* game)
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

// Draws the players
static void draw_players(Game* game)
{
    for (unsigned int i = 0; i < game->n_players; ++i)
    {
        Player player = game->players[i];
        unsigned int r, g, b;
        color_hex_to_rgb(player.color, &r, &g, &b);
        paint_pacman(vec_get_x(player.pacman_pos), vec_get_y(player.pacman_pos), r, g, b);
        paint_monster(vec_get_x(player.monster_pos), vec_get_y(player.monster_pos), r, g, b);
    }
}

int main (void)
{
    // Initialize the game structure
    // Initialized to zero due to the nature of malloc_check()
    Game* game = malloc_check(sizeof(Game));

    // RNG seeding (for player placement upon connection)
    srand(time(NULL));

    // Initialize the mutex
    if (pthread_mutex_init(&player_array_lock, NULL))
    {
        perror("ERROR - Mutex init failed");
        exit(EXIT_FAILURE);
    }

    // Read the board
    unsigned int n_empty = read_board(game, "board-drawer/board.txt");

    // Calculate maximum number of clients
    // Each client has two characters that occupy two spaces
    game->max_players = n_empty / (int)2;

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
        draw_players(game);
        render_board();
	}

    pthread_kill(connect_to_clients_thread, SIGUSR1);    
    pthread_join(connect_to_clients_thread, NULL);

    return 0;
}