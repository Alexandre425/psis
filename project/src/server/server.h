#pragma once

#include <stdlib.h>

#include "../common/utilities.h"

typedef struct _Player Player;

typedef struct _Game Game;

// Returns the game board
Board* game_get_board(Game* game);


// Creates a player and returns their player_id
unsigned int player_create(Game* game);
// Removes a player from the game
void player_destroy(Game* game, unsigned int player_id);

// Sets a player's color
void player_set_color(Player* player, Color color);

// Returns a player struct by its ID
Player* player_find_by_id(Game* game, unsigned int player_id);

