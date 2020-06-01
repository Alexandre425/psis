#pragma once

#include <stdlib.h>
#include <stdbool.h>

#include "../common/utilities.h"
#include "../common/pacman.h"

typedef struct _Player Player;

typedef struct _Fruit Fruit;

typedef struct _Game Game;

// Signals the client app (main thread) to quit in an orderly manner
void client_quit(void);

// Returns the player array
// Puts the number of players in n_players
Player** game_get_player_array(Game* game, unsigned int* n_players);
// Returns the server's socket
int game_get_server_socket(Game* game);
// Returns the game board
Board* game_get_board(Game* game);
// Returns the number of fruit
unsigned int game_get_n_fruits(Game* game);
// Returns the fruit in the provided index
Fruit* game_get_fruit(Game* game, unsigned int index);

// Sets the client's player id
void game_set_player_id(Game* game, unsigned int player_id);
// Sets the game's board
void game_set_board(Game* game, Board* board);
// Sets the number of fruit and allocates space in the fruit array if necessary
void game_set_n_fruits(Game* game, unsigned int n_fruits);

// Sets a fruit's position
void fruit_set_pos(Fruit* fruit, int x, int y);
// Sets a fruit's type
void fruit_set_type(Fruit* fruit, unsigned int fruit_type);
// Sets wether a fruit is alive
void fruit_set_is_alive(Fruit* fruit, int is_alive);

// Creates a player and returns the pointer to it
Player* player_create(Game* game, unsigned int player_id);
// Removes a player from the game
void player_destroy(Game* game, unsigned int player_id);
// Returns a player struct by its ID, or NULL if it does not exist
Player* player_find_by_id(Game* game, unsigned int player_id);

// Returns a player's pacman's x coordinate
int player_get_pac_pos_x(Player* player);
// Returns a player's pacman's y coordinate
int player_get_pac_pos_y(Player* player);

// Sets a player's pacman position
void player_set_pac_pos(Player* player, int x, int y);
// Sets a player's monster x position
void player_set_mon_pos(Player* player, int x, int y);
// Sets a player's color
void player_set_color(Player* player, Color color);
// Sets a player's power up state
void player_set_power_up_state(Player* player, bool state);