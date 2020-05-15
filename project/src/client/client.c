#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <SDL2/SDL.h>
#include <string.h>
#include <netinet/in.h>
#include <assert.h>
#include <arpa/inet.h>
#include <stdbool.h>

#include "../common/UI_library.h"
#include "../common/pacman.h"
#include "client_connection.h"
#include "client_message.h"
#include "client.h"

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
    unsigned int n_players;
    Player* players;
    Fruit* fruits;
} Game;

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
        Player player = game->players[i];
        unsigned int r, g, b;
        color_hex_to_rgb(player.color, &r, &g, &b);
        paint_pacman(vec_get_x(player.pacman_pos), vec_get_y(player.pacman_pos), r, g, b);
        paint_monster(vec_get_x(player.monster_pos), vec_get_y(player.monster_pos), r, g, b);
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
static void handle_user_input(int server_socket, SDL_Event event)
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

    // This part handles the movement for the Pacman, that is controlled with the mouse

    static char last_move_pac = -1;
    static char curr_move_pac = -1;
    int mouse_x = 0, mouse_y = 0, board_x = 0, board_y = 0;
    // If the user is pressing LMB
    if (SDL_GetMouseState(&mouse_x, &mouse_y) & SDL_BUTTON(SDL_BUTTON_LEFT))
    {
        get_board_place(mouse_x, mouse_y, &board_x, &board_y);

    }
    else
    {
        
    }
    // Pacman's movement intent during the previous frame
    // If there is anything on the stack
    if (n_pressed > 0)
    {

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

    int server_socket = connect_to_server(ip_str, port_str);

    // Create the game struct
    Game* game = malloc_check(sizeof(Game));

    // Trade the initial necessary info with the server
    message_send_color(server_socket, color);
    message_recv_board(server_socket, &game->board);

    create_board_window(board_get_size_x(game->board), board_get_size_y(game->board));

    // Poll and draw loop
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

        handle_user_input(server_socket, event);
        // Clear the board to render over
        clear_board(board_get_size_x(game->board), board_get_size_y(game->board));
        draw_bricks(game);
        draw_players(game);
        render_board();
	}

    shutdown(server_socket, SHUT_RDWR);

    return 0;
}