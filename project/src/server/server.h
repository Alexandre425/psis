#pragma once

#include <stdlib.h>
#include <stdbool.h>

#include "../common/utilities.h"

typedef struct _Player Player;

typedef struct _Game Game;

// Returns the game board
Board* game_get_board(Game* game);
// Returns the player array
// Puts the number of players in n_players
Player** game_get_player_array(Game* game, unsigned int* n_players);

// Creates a player and returns their player_id
unsigned int player_create(Game* game);
// Removes a player from the game
void player_destroy(Game* game, unsigned int player_id);

// Returns a player's ID
unsigned int player_get_id(Player* player);
// Returns a player's pacman x position
int player_get_pac_pos_x(Player* player);
// Returns a player's pacman y position
int player_get_pac_pos_y(Player* player);
// Returns a player's monster x position
int player_get_mon_pos_x(Player* player);
// Returns a player's monster y position
int player_get_mon_pos_y(Player* player);
// Returns a player's color
Color player_get_color(Player* player);
// Returns a player's power up state
bool player_get_power_up_state(Player* player);

// Sets a player's color
void player_set_color(Player* player, Color color);
void player_set_pac_move_dir(Player* player, char move_dir);
void player_set_mon_move_dir(Player* player, char move_dir);

// Returns a player struct by its ID, or NULL if it does not exist
Player* player_find_by_id(Game* game, unsigned int player_id);

