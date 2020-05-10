#pragma once

#include <stdlib.h>

typedef struct _Player Player;

typedef struct _Game Game;



// Creates a player and returns their player_id
unsigned int player_create(Game* game);
// Removes a player from the game
void player_destroy(Game* game, unsigned int player_id);
// Returns a player struct by its ID
Player* player_find_by_id(Game* game, unsigned int player_id);

