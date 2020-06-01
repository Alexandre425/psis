#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <SDL2/SDL.h>
#include <string.h>
#include <netinet/in.h>
#include <assert.h>
#include <arpa/inet.h>

#include "../common/UI_library.h"
#include "client_connection.h"
#include "client_message.h"
#include "client.h"

// Used by other threads to stop the main loop and quit
int quit = 0;
void client_quit(void)
{quit = 1;}

typedef struct _Player
{
    unsigned int player_id;
    Color color;
    int powered_up;
    Vector* pacman_pos;
    Vector* monster_pos;
} Player;

typedef struct _Fruit
{
    unsigned int fruit_type;    // Cherry or lemon
    int is_alive;               // Is it on the board? (Or waiting to respawn)
    Vector* pos;
} Fruit;

typedef struct _Game
{
    int server_socket;
    unsigned int player_id;
    Board* board;
    unsigned int n_players;
    Player** players;
    unsigned int n_fruits;
    Fruit* fruits;
} Game;

int game_get_server_socket(Game* game)
{
    return game->server_socket;
}
Board* game_get_board(Game* game)
{
    return game->board;
}
unsigned int game_get_n_fruits(Game* game)
{
    return game->n_fruits;
}
Fruit* game_get_fruit(Game* game, unsigned int index)
{
    return &game->fruits[index];
}

void game_set_player_id(Game* game, unsigned int player_id)
{
    game->player_id = player_id;
}
void game_set_board(Game* game, Board* board)
{
    game->board = board;
}
void game_set_n_fruits(Game* game, unsigned int n_fruits)
{
    if (n_fruits > game->n_fruits)
    {
        if (!game->fruits)          // If fruits were created
            game->fruits = malloc_check(n_fruits * sizeof(Fruit));  // This already zeroes things out :)
        else
        {
            unsigned int diff = n_fruits - game->n_fruits;
            game->fruits = realloc_check(game->fruits, n_fruits * sizeof(Fruit));
            memset(game->fruits + (game->n_fruits * sizeof(Fruit)), 0, diff * sizeof(Fruit));   // Basically zeroes out the memory added with the realloc
        }                                                                                       // it's bad, but I ain't allocating every fruit, nuh uh
    }
    if (n_fruits < game->n_fruits)  // If fruits were deleted (players left)
    {
        unsigned int diff = game->n_fruits - n_fruits;  // Find how many fruits were deleted
        for (unsigned int i = 0; i < diff; ++i)         // Free that many fruit in the end of the array which 
        {                                               //  prevents a memory leak with unused vectors
            free(game->fruits[game->n_fruits - 1 - i].pos);     // r/badcode worthy? oof
        }
    }
    game->n_fruits = n_fruits;
}


void fruit_set_pos(Fruit* fruit, int x, int y)
{
    if (!fruit->pos)
        fruit->pos = vec_create(x, y);
    else
        vec_set(fruit->pos, x, y);
}
void fruit_set_type(Fruit* fruit, unsigned int fruit_type)
{
    fruit->fruit_type = fruit_type;
}
void fruit_set_is_alive(Fruit* fruit, int is_alive)
{
    fruit->is_alive = is_alive;
}


Player* player_find_by_id(Game* game, unsigned int player_id)
{
    for (unsigned int i = 0; i < game->n_players; ++i)
    {
        if (game->players[i]->player_id == player_id)
        {
            return game->players[i];
        }
    }
    return NULL;
}

Player** game_get_player_array(Game* game, unsigned int* n_players)
{
    *n_players = game->n_players;
    return game->players;
}

Player* player_create(Game* game, unsigned int player_id)
{
    // If no players exist (array uninitialized)
    if (!game->players)
    {
        game->n_players++;
        game->players = malloc_check(sizeof(Player*));
    }
    else
    {
        game->n_players++;
        game->players = realloc_check(game->players, game->n_players * sizeof(Player*));
    }
    Player* new_player = malloc_check(sizeof(Player));
    game->players[game->n_players-1] = new_player;
    // Default color (black)
    new_player->color = 0x000000;
    // Assign the player_id
    new_player->player_id = player_id;

    // Put the characters in an invalid position (to be set later)
    new_player->pacman_pos = vec_create(-1, -1);
    new_player->monster_pos = vec_create(-1, -1);

    return new_player;
}

void player_destroy(Game* game, unsigned int player_id)
{
    // Get the ptr to the player struct
    Player* player = player_find_by_id(game, player_id);

    // Free the position vectors
    free(player->pacman_pos);
    free(player->monster_pos);

    // Overwrite the player to be destroyed with the last player
    memcpy(player, game->players[game->n_players-1], sizeof(Player));

    // Free the player struct
    free(game->players[game->n_players-1]);

    game->n_players--;
    // Not strictly necessary, but prevents a bug with player_create where
    // the second client joining an empty server gets assigned player_id 2
    // instead of 1 (as the array still contains the data after the realloc)
    if (game->n_players == 0)
    {
        free(game->players);
        game->players = NULL;
    }
}

int player_get_pac_pos_x(Player* player)
{
    return vec_get_x(player->pacman_pos);
}
int player_get_pac_pos_y(Player* player)
{
    return vec_get_y(player->pacman_pos);
}

void player_set_pac_pos(Player* player, int x, int y)
{
    vec_set(player->pacman_pos, x, y);
}
void player_set_mon_pos(Player* player, int x, int y)
{
    vec_set(player->monster_pos, x, y);
}
void player_set_color(Player* player, Color color)
{
    player->color = color;
}
void player_set_power_up_state(Player* player, bool state)
{
    player->powered_up = state;
}


// Draws the bricks
static void draw_bricks(Game* game)
{
    for (unsigned int i = 0; i < board_get_size_x(game->board); ++i)
    {
        for (unsigned int j = 0; j < board_get_size_y(game->board); ++j)
        {
            if (board_get_tile(game->board, i, j) == TILE_BRICK)
                paint_brick(i, j);
        }
    }
}

// Draws the players
static void draw_players(Game* game)
{
    for (unsigned int i = 0; i < game->n_players; ++i)
    {
        Player* player = game->players[i];
        unsigned int r, g, b;
        color_hex_to_rgb(player->color, &r, &g, &b);
        paint_pacman(vec_get_x(player->pacman_pos), vec_get_y(player->pacman_pos), r, g, b, player->powered_up);
        paint_monster(vec_get_x(player->monster_pos), vec_get_y(player->monster_pos), r, g, b);
    }
}

// Draws the fruit
static void draw_fruit(Game* game)
{
    for (unsigned int i = 0; i < game->n_fruits; ++i)
    {
        Fruit* fruit = &game->fruits[i];
        if (!fruit->is_alive)
            continue;
        if (fruit->fruit_type == FRUIT_CHERRY)
            paint_cherry(vec_get_x(fruit->pos), vec_get_y(fruit->pos));
        else
            paint_lemon(vec_get_x(fruit->pos), vec_get_y(fruit->pos));
    }
}

static void update_key(char* wasd_stack, unsigned int* wasd_pressed, unsigned int* n_pressed, char key, Uint8 state)
{
    // If the key is newly pressed
    if (state)
    {
        // Update the pressed key array and push to WASD stack
        switch (key)
        {
        case 'w':
            wasd_pressed[0] = 1;
            break;
        case 'a':
            wasd_pressed[1] = 1;
            break;
        case 's':
            wasd_pressed[2] = 1;
            break;
        case 'd':
            wasd_pressed[3] = 1;
            break;
        default:
            break;
        }
        wasd_stack[*n_pressed] = key;
        *n_pressed = *n_pressed + 1;
    }
    // If the key is newly released
    else
    {
        // Update the pressed key array
        switch (key)
        {
        case 'w':
            wasd_pressed[0] = 0;
            break;
        case 'a':
            wasd_pressed[1] = 0;
            break;
        case 's':
            wasd_pressed[2] = 0;
            break;
        case 'd':
            wasd_pressed[3] = 0;
            break;
        default:
            break;
        }
        // Remove from WASD stack
        for (unsigned int i = 0; i < *n_pressed; ++i)
        {
            // Find the key
            if (wasd_stack[i] == key)
            {
                // Shift remaining keys down
                for (; i < *n_pressed-1; ++i)
                {
                    wasd_stack[i] = wasd_stack[i+1];
                }
                break;
            }
        }
        *n_pressed = *n_pressed - 1;
    }
    
}

// Handles the user input and sends movement intent to the server
static void handle_user_input(int server_socket, Game* game ,SDL_Event event)
{
    // This part handles the movement for the monster, that is controlled with WASD

    // Stores the WASD keys in order of them being pressed
    // They are removed once released
    static char wasd_stack[4] = {0,0,0,0};
    // Stores which of they keys is currently pressed as a 1
    // W is addr 0, A is addr 1,...
    // WASD
    static unsigned int wasd_pressed[4] = {0,0,0,0};
    static unsigned int n_pressed = 0;

    const Uint8* keys_pressed = SDL_GetKeyboardState(NULL);

    // If the state of the a key differs from the last frame
    if (keys_pressed[SDL_SCANCODE_W] != wasd_pressed[0])
    {
        update_key(wasd_stack, wasd_pressed, &n_pressed, 'w', keys_pressed[SDL_SCANCODE_W]);
    }
    else if (keys_pressed[SDL_SCANCODE_A] != wasd_pressed[1])
    {
        update_key(wasd_stack, wasd_pressed, &n_pressed, 'a', keys_pressed[SDL_SCANCODE_A]);
    }
    else if (keys_pressed[SDL_SCANCODE_S] != wasd_pressed[2])
    {
        update_key(wasd_stack, wasd_pressed, &n_pressed, 's', keys_pressed[SDL_SCANCODE_S]);
    }
    else if (keys_pressed[SDL_SCANCODE_D] != wasd_pressed[3])
    {
        update_key(wasd_stack, wasd_pressed, &n_pressed, 'd', keys_pressed[SDL_SCANCODE_D]);
    }

    // The monster's last and current movement directions
    static char last_move_mon = (char)-1;
    static char curr_move_mon = (char)-1;
    // Update the current movement direction
    if (n_pressed)                                      // If any keys are pressed, movement is the last pressed key
        curr_move_mon =  wasd_stack[n_pressed - 1];
    else                                                // Otherwise the monster stops
        curr_move_mon = (char)0;
    // If there is a new movement direction (different from the last one sent)
    if (curr_move_mon != last_move_mon)
    {
        message_send_movement_mon(server_socket, curr_move_mon);
        last_move_mon = curr_move_mon;
    }
    
    // This part handles the movement for the Pacman, that is controlled with the mouse

    static char last_move_pac = -1;
    static char curr_move_pac = -1;
    int mouse_x = 0, mouse_y = 0, board_x = 0, board_y = 0;
    // If the user is pressing LMB
    if (SDL_GetMouseState(&mouse_x, &mouse_y) & SDL_BUTTON(SDL_BUTTON_LEFT))
    {
        get_board_place(mouse_x, mouse_y, &board_x, &board_y);
        Player* player = player_find_by_id(game, game->player_id);
        int pac_x = player_get_pac_pos_x(player), pac_y = player_get_pac_pos_y(player);
        // Compute the distance between the pacman and the tile the mouse points to
        // The pacman will move in the direction which shows the biggest discrepancy, or not move if null distance
        if (board_x == pac_x && board_y == pac_y)                       // If the mouse is over the pacman
        {
            curr_move_pac = (char)0;
        }
        else if (abs_int(board_x - pac_x) > abs_int(board_y - pac_y))   // If further in the x direction
        {
            if (board_x > pac_x)
                curr_move_pac = 'd';
            else
                curr_move_pac = 'a';
        }
        else                                                            // If further in the y direction
        {
            if (board_y > pac_y)
                curr_move_pac = 's';
            else
                curr_move_pac = 'w';
        }
    }
    else
    {
        // Tell the server to stop the pacman
        curr_move_pac = (char)0;
    }

    // If there is a new movement direction
    if (curr_move_pac != last_move_pac)
    {
        message_send_movement_pac(server_socket, curr_move_pac);
        last_move_pac = curr_move_pac;
    }
}

int main (int argc, char* argv[])
{
    // Verify correct argument usage
    if (argc != 3)
    {
        puts("Usage: ./client <ip>:<port> <color (hex RRGGBB format)>");
        exit(EXIT_FAILURE);
    }
    // Read server IP and port from string
    char* ip_str = strtok(argv[1], ":");
    char* port_str = strtok(NULL, ":");
    // Read the color
    Color color;
    sscanf(argv[2], "%x", &color);

    // Connect to the server and send the color
    int server_socket = connect_to_server(ip_str, port_str);
    message_send_color(server_socket, color);

    // Create the game struct
    Game* game = malloc_check(sizeof(Game));
    game->server_socket = server_socket;

    // Thread to handle incoming messages from the server
    pthread_t recv_from_server_thread;
    pthread_create(&recv_from_server_thread, NULL, recv_from_server, (void*)game);

    // Wait for the board and players to be received from the server (happens in another thread)
    // Burns a couple of cycles, but its better than pointlessly learning pthread_cond_wait() for one thing
    while (!game->board || !game->players)
        usleep(1000);

    create_board_window(board_get_size_x(game->board), board_get_size_y(game->board));

    // Poll and draw loop
    SDL_Event event;
    while (!quit){
		while (SDL_PollEvent(&event)) {
            switch (event.type)
            {
            case SDL_QUIT:          // Quit the program when the window is closed
                close_board_windows();
                quit = 1;
                break;
            default:
                break;
            }
		}

        handle_user_input(server_socket, game, event);
        // Clear the board to render over
        clear_board(board_get_size_x(game->board), board_get_size_y(game->board));
        draw_bricks(game);
        draw_players(game);
        draw_fruit(game);
        render_board();
	}

    shutdown(server_socket, SHUT_RDWR);
    pthread_cancel(recv_from_server_thread);

    return 0;
}