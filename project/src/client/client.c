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

        // Clear the board to render over
        clear_board(board_get_size_x(game->board), board_get_size_y(game->board));
        draw_bricks(game);
        draw_players(game);
        render_board();
	}

    shutdown(server_socket, SHUT_RDWR);

    return 0;
}