#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <pthread.h>
#include <SDL2/SDL.h>
#include <signal.h>
#include <time.h>

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
    int powered_up;                         // Stores 0, 1 or 2, depending on how many monsters the pacman can still eat
    unsigned int score;                     // The number of things eaten
    Vector* pacman_pos;
    char pacman_move_dir;                   // Movement direction
    struct timespec pacman_last_move_time;  // Time of last movement
    Vector* monster_pos;
    char monster_move_dir;
    struct timespec monster_last_move_time;
} Player;

typedef struct _Fruit
{
    unsigned int fruit_type;    // Cherry or lemon
    int is_alive;               // Is it on the board? (Or waiting to respawn)
    struct timespec eaten_time; // Time it was last eaten
    Vector* pos;
} Fruit;

typedef struct _Game
{
    Board* board;
    unsigned int max_players;
    unsigned int n_players;
    Player** players;
    unsigned int n_fruits;
    Fruit** fruits;
} Game;

Board* game_get_board(Game* game)
{
    return game->board;
}

Player** game_get_player_array(Game* game, unsigned int* n_players)
{
    *n_players = game->n_players;
    return game->players;
}

Fruit** game_get_fruit_array(Game* game, unsigned int* n_fruits)
{
    *n_fruits = game->n_fruits;
    return game->fruits;
}
int game_is_full(Game* game)
{
    return game->n_players == game->max_players;
}

// Creates a fruit and returns the pointer to the alloc'd struct
static Fruit* fruit_create(Game* game, unsigned int fruit_type)
{
    Fruit* fruit = malloc_check(sizeof(Fruit));
    fruit->fruit_type = fruit_type;
    fruit->is_alive = 1;
    int x, y;                                       // Put the fruit in an empty space
    board_random_empty_space(game->board, &x, &y);
    fruit->pos = vec_create(x, y);
    board_set_tile(game->board, x, y, TILE_FRUIT);  // Put it on the board
    return fruit;
}

// Destroys a fruit
static void fruit_destroy(Game* game, Fruit* fruit)
{
    if (fruit->is_alive)
        board_set_tile(game->board, vec_get_x(fruit->pos), vec_get_y(fruit->pos), TILE_EMPTY);
    free(fruit->pos);
    free(fruit);
}

int fruit_get_pos_x(Fruit* fruit)
{
    return vec_get_x(fruit->pos);
}
int fruit_get_pos_y(Fruit* fruit)
{
    return vec_get_y(fruit->pos);
}
int fruit_get_is_alive(Fruit* fruit)
{
    return fruit->is_alive;
}
unsigned int fruit_get_type(Fruit* fruit)
{
    return fruit->fruit_type;
}


// Respawns a fruit
static void fruit_respawn(Game* game, Fruit* fruit)
{
    fruit->is_alive = 1;
    int x, y;
    board_random_empty_space(game->board, &x, &y);
    vec_set(fruit->pos, x, y);
    board_set_tile(game->board, x, y, TILE_FRUIT);
}

// Returns the fruit on given position
static Fruit* fruit_find_by_pos(Game* game, int x, int y)
{
    for (unsigned int i = 0; i < game->n_fruits; ++i)
    {
        Fruit* fruit = game->fruits[i];
        if (x == vec_get_x(fruit->pos) && y == vec_get_y(fruit->pos))
            return fruit;
    }
    puts("ERROR - Attempted to find fruit in a position where there is none");
    exit(EXIT_FAILURE);
}

// Updates the number of fruit on the board and reallocs the fruit array if necessary
static void fruit_count_update(Game* game, unsigned int n_players)
{
    int new_n_fruits = max(2 * (n_players - 1), 0);

    if (new_n_fruits > game->n_fruits)      // Number of fruits has increased
    {
        if (game->fruits == NULL)
            game->fruits = malloc_check(new_n_fruits * sizeof(Fruit));
        else
            game->fruits = realloc_check(game->fruits, new_n_fruits * sizeof(Fruit));
        game->fruits[new_n_fruits - 1] = fruit_create(game, FRUIT_CHERRY);     // Create two new fruits
        game->fruits[new_n_fruits - 2] = fruit_create(game, FRUIT_LEMON);
    }
    if (new_n_fruits < game->n_fruits)      // Number of fruits has decreased
    {
        fruit_destroy(game, game->fruits[game->n_fruits - 1]);                 // Destroy two fruits
        fruit_destroy(game, game->fruits[game->n_fruits - 2]);
    }
    
    game->n_fruits = new_n_fruits;
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

unsigned int player_create(Game* game)
{
    pthread_mutex_lock(&player_array_lock);
    game->n_players++;
    // If no players exist (array uninitialized)
    if (!game->players)
        game->players = malloc_check(sizeof(Player*));
    else
        game->players = realloc_check(game->players, game->n_players * sizeof(Player*));

    Player* new_player = malloc_check(sizeof(Player));
    game->players[game->n_players-1] = new_player;
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
    int x, y;
    board_random_empty_space(game->board, &x, &y);  // Pacman
    new_player->pacman_pos = vec_create(x, y);
    board_set_tile(game->board, x, y, board_player_id_to_tile_type(player_id, 1));

    board_random_empty_space(game->board, &x, &y);  // Monster
    new_player->monster_pos = vec_create(x, y);
    board_set_tile(game->board, x, y, board_player_id_to_tile_type(player_id, 0));

    // Initialize the move timer
    clock_gettime(CLOCK_MONOTONIC, &new_player->monster_last_move_time);
    clock_gettime(CLOCK_MONOTONIC, &new_player->pacman_last_move_time);

    // Update the fruit number
    fruit_count_update(game, game->n_players);

    pthread_mutex_unlock(&player_array_lock);

    return player_id;
}

void player_destroy(Game* game, unsigned int player_id)
{
    pthread_mutex_lock(&player_array_lock);

    // Get the ptr to the player struct
    Player* player = player_find_by_id(game, player_id);
    
    // Clear the spaces on the board occupied by the pacman and monster
    board_set_tile(game->board, vec_get_x(player->pacman_pos), vec_get_y(player->pacman_pos), TILE_EMPTY);
    board_set_tile(game->board, vec_get_x(player->monster_pos), vec_get_y(player->monster_pos), TILE_EMPTY);

    // Free the position vectors
    free(player->pacman_pos);
    free(player->monster_pos);

    // Overwrite the player to be destroyed with the last player
    memcpy(player, game->players[game->n_players-1], sizeof(Player));

    // Free the last player
    free(game->players[game->n_players-1]);
    game->n_players--;
    // Reset the score if only one player remains
    if (game->n_players == 1)
        game->players[0]->score = 0;

    // Not strictly necessary, but prevents a bug with player_create where
    // the second client joining an empty server gets assigned player_id 2
    // instead of 1 (as the array still contains the data after the realloc)
    if (game->n_players == 0)
    {
        free(game->players);
        game->players = NULL;
    }
    
    // Update the fruit number
    fruit_count_update(game, game->n_players);
    
    pthread_mutex_unlock(&player_array_lock);
}

unsigned int player_get_id(Player* player)
{
    return player->player_id;
}
unsigned int player_get_score(Player* player)
{
    return player->score;
}
int player_get_pac_pos_x(Player* player)
{
    return vec_get_x(player->pacman_pos);
}
int player_get_pac_pos_y(Player* player)
{
    return vec_get_y(player->pacman_pos);
}
int player_get_mon_pos_x(Player* player)
{
    return vec_get_x(player->monster_pos);
}
int player_get_mon_pos_y(Player* player)
{
    return vec_get_y(player->monster_pos);
}
Color player_get_color(Player* player)
{
    return player->color;
}
int player_get_power_up_state(Player* player)
{
    return player->powered_up;
}

void player_set_color(Player* player, Color color)
{
    player->color = color;
}
void player_set_pac_move_dir(Player* player, char move_dir)
{
    player->pacman_move_dir = move_dir;
}
void player_set_mon_move_dir(Player* player, char move_dir)
{
    player->monster_move_dir = move_dir;
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
                    board_set_tile(game->board, j, i, TILE_EMPTY);
                    n_empty++;
                break;

                case 'B':
                    board_set_tile(game->board, j, i, TILE_BRICK);
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
        Fruit* fruit = game->fruits[i];
        if (!fruit->is_alive)
            continue;
        if (fruit->fruit_type == FRUIT_CHERRY)
            paint_cherry(vec_get_x(fruit->pos), vec_get_y(fruit->pos));
        else
            paint_lemon(vec_get_x(fruit->pos), vec_get_y(fruit->pos));
    }
}

// Determines the target coordinates of a character 
// Translates between wasd and up left down right
// tgt_x and y should be set to the current tile, but will be modified to the target tile
static void get_target_tile(int* tgt_x, int* tgt_y, char direction)
{
    switch (direction)
    {
    case 'w':
        *tgt_y = *tgt_y - 1;
        break;
    case 'a':
        *tgt_x = *tgt_x - 1;
        break;
    case 's':
        *tgt_y = *tgt_y + 1;
        break;
    case 'd':
        *tgt_x = *tgt_x + 1;
        break;
    default:
        break;
    }
}

// Same as above, but the opposite direction
// Used for bouncing back from the wall
static void get_opposite_target_tile(int* tgt_x, int* tgt_y, char direction)
{
    switch (direction)
    {
    case 'w':
        *tgt_y = *tgt_y + 1;
        break;
    case 'a':
        *tgt_x = *tgt_x + 1;
        break;
    case 's':
        *tgt_y = *tgt_y - 1;
        break;
    case 'd':
        *tgt_x = *tgt_x - 1;
        break;
    default:
        break;
    }
}

// Handles a character eating another
// Parameters are the coordinates of the eater and eaten characters
// If the eater is moving (i.g. Pacman with teeth moves into an enemy monster), eater_is_moving should be 1
// Otherwise (i.g. regular Pacman moves into an enemy monster) it should be 0
static void handle_character_eat(Game* game, int eater_x, int eater_y, int eaten_x, int eaten_y, int eater_is_moving)
{
    int res_x, res_y;
    unsigned int eaten_tile = board_get_tile(game->board, eaten_x, eaten_y);
    unsigned int eater_tile = board_get_tile(game->board, eater_x, eater_y);
    Player* eaten_player = player_find_by_id(game, board_tile_type_to_player_id(eaten_tile));
    Player* eater_player = player_find_by_id(game, board_tile_type_to_player_id(eater_tile));
    if (eater_is_moving)
    {
        board_set_tile(game->board, eater_x, eater_y, TILE_EMPTY);  // Mark the eater's tile as a valid random tile to respawn in
        board_random_empty_space(game->board, &res_x, &res_y);      // Determine the random respawn position
        if (board_tile_type_is_pacman(eaten_tile) == 1)             // If a monster eats a Pacman
        {
            vec_set(eaten_player->pacman_pos, res_x, res_y);        // Pacman respawns
            vec_set(eater_player->monster_pos, eaten_x, eaten_y);   // Put the monster in the Pacman's position
        }
        else                                                        // If a Pacman eats a monster
        {
            vec_set(eaten_player->monster_pos, res_x, res_y);       // Monster respawns
            vec_set(eater_player->pacman_pos, eaten_x, eaten_y);    // Put the Pacman in the monster's position
        }
        board_set_tile(game->board, eaten_x, eaten_y, eater_tile);  // Move things on the board
        board_set_tile(game->board, res_x, res_y, eaten_tile);                                                       
    }
    else
    {
        board_set_tile(game->board, eaten_x, eaten_y, TILE_EMPTY);  // Mark the eaten's tile as a valid random tile to respawn in
        board_random_empty_space(game->board, &res_x, &res_y);      // Determine the random respawn position
        Player* eaten_player = player_find_by_id(game, board_tile_type_to_player_id(eaten_tile));
        if (board_tile_type_is_pacman(eaten_tile) == 1)             // If a monster eats a Pacman
        {
            vec_set(eaten_player->pacman_pos, res_x, res_y);        // Pacman respawns
        }
        else                                                        // If a Pacman eats a monster
        {
            vec_set(eaten_player->monster_pos, res_x, res_y);       // Monster respawns
            puts("uh oh");
        }
        board_set_tile(game->board, res_x, res_y, eaten_tile);      // Move things on the board                                             
    }
    eater_player->score++;
}

// Handles a character eating a fruit
static void handle_fruit_eat(Game* game, Fruit* fruit, Player* player, int is_pacman)
{
    int fruit_x, fruit_y, player_x, player_y;
    vec_get(fruit->pos, &fruit_x, &fruit_y); 
    if (is_pacman)  // Move the character in question
    {
        vec_get(player->pacman_pos, &player_x, &player_y);
        vec_set(player->pacman_pos, vec_get_x(fruit->pos), vec_get_y(fruit->pos));
        player->powered_up = 2;
    }
    else
    {
        vec_get(player->monster_pos, &player_x, &player_y);
        vec_set(player->monster_pos, vec_get_x(fruit->pos), vec_get_y(fruit->pos));
    }
    board_set_tile(game->board, fruit_x, fruit_y, board_get_tile(game->board, player_x, player_y)); // Move things on the board
    board_set_tile(game->board, player_x, player_y, TILE_EMPTY);

    player->score++;
    fruit->is_alive = 0;    // Set the time the fruit was eaten
    clock_gettime(CLOCK_MONOTONIC, &fruit->eaten_time);
}

// Handles movement for the pacman
static void handle_pacman_move(Game* game, Player* player, int tgt_x, int tgt_y)
{
    int curr_x = vec_get_x(player->pacman_pos), curr_y = vec_get_y(player->pacman_pos); // The current position
    unsigned int tile = board_get_tile(game->board, tgt_x, tgt_y);
    if (board_is_oob(game->board, tgt_x, tgt_y))        // If the target is OOB (out of bounds)
        tile = TILE_BRICK;                              // Handle collisions as if the target is a brick (bounce back)
    switch (tile)
    {
    case TILE_EMPTY:            // Simply move into the empty tile
        board_set_tile(game->board, tgt_x, tgt_y, board_get_tile(game->board, curr_x, curr_y)); // Move the pacman in the board
        vec_set(player->pacman_pos, tgt_x, tgt_y);                                              // and in the player struct
        board_set_tile(game->board, curr_x, curr_y, TILE_EMPTY);                                // Empty the tile behind it
        clock_gettime(CLOCK_MONOTONIC, &player->pacman_last_move_time);                         // Update the last move time
        break;

    case TILE_FRUIT:
    {
        Fruit* fruit = fruit_find_by_pos(game, tgt_x, tgt_y);
        handle_fruit_eat(game, fruit, player, 1);
        clock_gettime(CLOCK_MONOTONIC, &player->pacman_last_move_time);    
        break;
    }

    case TILE_BRICK:            // Bounce back (if able)
        tgt_x = curr_x; tgt_y = curr_y;
        get_opposite_target_tile(&tgt_x, &tgt_y, player->pacman_move_dir);                                          // Invert the movement direction
        if (board_is_oob(game->board, tgt_x, tgt_y) || board_get_tile(game->board, tgt_x, tgt_y) == TILE_BRICK);    // If the new target tile is OOB or a brick
            // Stay in place, do nothing
        else
            handle_pacman_move(game, player, tgt_x, tgt_y);                                                         // Handle the new target tile
        break;

    default:                    // Could be a friendly or enemy
        if (board_tile_type_to_player_id(tile) == player->player_id)    // If friendly monster
        {
            board_set_tile(game->board, tgt_x, tgt_y, board_get_tile(game->board, curr_x, curr_y)); // Swap the characters on the board
            board_set_tile(game->board, curr_x, curr_y, tile);                                      
            Vector* tmp_vec = player->pacman_pos;                                                   // Swap the character positions (on the struct)
            player->pacman_pos = player->monster_pos;                                               // Simply swapping the pointer works
            player->monster_pos = tmp_vec;
            clock_gettime(CLOCK_MONOTONIC, &player->pacman_last_move_time);                         // Update the last move time
        }
        else if (board_tile_type_is_pacman(tile) == 1)  // If other pacman
        {
            board_set_tile(game->board, tgt_x, tgt_y, board_get_tile(game->board, curr_x, curr_y)); // Swap the characters on the board
            board_set_tile(game->board, curr_x, curr_y, tile);                    
            Vector* tmp_vec = player->pacman_pos;                                                   // Swap the character positions (on the struct)
            Player* target_player = player_find_by_id(game, board_tile_type_to_player_id(tile));
            player->pacman_pos = target_player->pacman_pos;
            target_player->pacman_pos = tmp_vec;
            clock_gettime(CLOCK_MONOTONIC, &player->pacman_last_move_time);                         // Update the last move time
        }
        else    // If enemy monster
        {
            if (player->powered_up)     // Pacman has teeth -> it eats the monster
            {
                handle_character_eat(game, curr_x, curr_y, tgt_x, tgt_y, 1);
                player->powered_up = player->powered_up - 1;
                clock_gettime(CLOCK_MONOTONIC, &player->pacman_last_move_time);
            }
            else                        // Pacman forgot his dentures -> the monster eats it
            {
                handle_character_eat(game, tgt_x, tgt_y, curr_x, curr_y, 0);
                clock_gettime(CLOCK_MONOTONIC, &player->pacman_last_move_time);
            }
        }
        
        break;
    }
}
// Handles movement for the monster
static void handle_monster_move(Game* game, Player* player, int tgt_x, int tgt_y)
{
    int curr_x = vec_get_x(player->monster_pos), curr_y = vec_get_y(player->monster_pos);   // The current position
    unsigned int tile = board_get_tile(game->board, tgt_x, tgt_y);
    if (board_is_oob(game->board, tgt_x, tgt_y))        // If the target is OOB (out of bounds)
        tile = TILE_BRICK;                              // Handle collisions as if the target is a brick (bounce back)
    
    switch (tile)
    {
    case TILE_EMPTY:
        board_set_tile(game->board, tgt_x, tgt_y, board_get_tile(game->board, curr_x, curr_y)); // Move the monster in the board
        vec_set(player->monster_pos, tgt_x, tgt_y);                                             // and in the player struct
        board_set_tile(game->board, curr_x, curr_y, TILE_EMPTY);                                // Empty the tile behind it
        clock_gettime(CLOCK_MONOTONIC, &player->monster_last_move_time);                        // Update the last move time
        break;

    case TILE_FRUIT:
    {
        Fruit* fruit = fruit_find_by_pos(game, tgt_x, tgt_y);
        handle_fruit_eat(game, fruit, player, 0);
        clock_gettime(CLOCK_MONOTONIC, &player->monster_last_move_time);    
        break;
    }

    case TILE_BRICK:
        tgt_x = curr_x; tgt_y = curr_y;
        get_opposite_target_tile(&tgt_x, &tgt_y, player->monster_move_dir);                                         // Invert the movement direction
        if (board_is_oob(game->board, tgt_x, tgt_y) || board_get_tile(game->board, tgt_x, tgt_y) == TILE_BRICK);    // If the new target tile is OOB or a brick
            // Stay in place, do nothing
        else
            handle_monster_move(game, player, tgt_x, tgt_y);                                                        // Handle the new target tile
        break;
    
    default:
        if (board_tile_type_to_player_id(tile) == player->player_id)    // If friendly pacman
        {
            board_set_tile(game->board, tgt_x, tgt_y, board_get_tile(game->board, curr_x, curr_y)); // Swap the characters on the board
            board_set_tile(game->board, curr_x, curr_y, tile);                                      
            Vector* tmp_vec = player->pacman_pos;                                                   // Swap the character positions (on the struct)
            player->pacman_pos = player->monster_pos;                                               // Simply swapping the pointer works
            player->monster_pos = tmp_vec;
            clock_gettime(CLOCK_MONOTONIC, &player->monster_last_move_time);                        // Update the last move time
        }
        else if (board_tile_type_is_pacman(tile) == 0)  // If enemy monster
        {
            board_set_tile(game->board, tgt_x, tgt_y, board_get_tile(game->board, curr_x, curr_y)); // Swap the characters on the board
            board_set_tile(game->board, curr_x, curr_y, tile);                    
            Vector* tmp_vec = player->monster_pos;                                                  // Swap the character positions (on the struct)
            Player* target_player = player_find_by_id(game, board_tile_type_to_player_id(tile));
            player->monster_pos = target_player->monster_pos;
            target_player->monster_pos = tmp_vec;
            clock_gettime(CLOCK_MONOTONIC, &player->monster_last_move_time);                        // Update the last move time
        }
        else    // If enemy pacman
        {
            Player* target_player = player_find_by_id(game, board_tile_type_to_player_id(tile));
            if (target_player->powered_up)  // Enemy pacman has teeth - monster dies
            {
                handle_character_eat(game, tgt_x, tgt_y, curr_x, curr_y, 0);
                target_player->powered_up = target_player->powered_up - 1;
                clock_gettime(CLOCK_MONOTONIC, &player->monster_last_move_time);
            }
            else                            // No teeth - enemy pacman dies
            {
                handle_character_eat(game, curr_x, curr_y, tgt_x, tgt_y, 1);
                clock_gettime(CLOCK_MONOTONIC, &player->monster_last_move_time);
            }
        }
        break;
    }
}

static void handle_inactivity_move(Game* game, Player* player, int is_pacman)
{
    int x, y;
    board_random_empty_space(game->board, &x, &y);
    if (x != -1)
    {
        int curr_x, curr_y;
        if (is_pacman)
        {
            curr_x = vec_get_x(player->pacman_pos); curr_y = vec_get_y(player->pacman_pos);
            vec_set(player->pacman_pos, x, y);
            clock_gettime(CLOCK_MONOTONIC, &player->pacman_last_move_time);
        }
        else
        {
            curr_x = vec_get_x(player->monster_pos); curr_y = vec_get_y(player->monster_pos);
            vec_set(player->monster_pos, x, y);
            clock_gettime(CLOCK_MONOTONIC, &player->monster_last_move_time);
        }
        board_set_tile(game->board, x, y, board_get_tile(game->board, curr_x, curr_y));
        board_set_tile(game->board, curr_x, curr_y, TILE_EMPTY);
    }
}

// Moves the characters, their interactions, handles fruit spawning and all things Pacman related
static void game_update(Game* game)
{
    // Move the characters if enough time has passed
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    for (unsigned int i = 0; i < game->n_players; ++i)
    {
        Player* player = game->players[i];
        if (time_diff_ms(player->monster_last_move_time, now) > 500 && player->monster_move_dir != (char)0) // If the last movement happened more than 500ms ago
        {                                                                                                   // and the monster wants to move
            int tgt_x = vec_get_x(player->monster_pos), tgt_y = vec_get_y(player->monster_pos); // Get the current position
            get_target_tile(&tgt_x, &tgt_y, player->monster_move_dir);                          // Get the target position

            handle_monster_move(game, player, tgt_x, tgt_y);
        }
        else if (time_diff_ms(player->monster_last_move_time, now) > 30000)     // Inactivity counter
        {
            handle_inactivity_move(game, player, 0);
        }
        if (time_diff_ms(player->pacman_last_move_time, now) > 500 && player->pacman_move_dir != (char)0)   // Same as above with pacman
        {
            int tgt_x = vec_get_x(player->pacman_pos), tgt_y = vec_get_y(player->pacman_pos);
            get_target_tile(&tgt_x, &tgt_y, player->pacman_move_dir);

            handle_pacman_move(game, player, tgt_x, tgt_y);
        }
        else if (time_diff_ms(player->pacman_last_move_time, now) > 30000)     // Inactivity counter
        {
            handle_inactivity_move(game, player, 1);
        }
    }
    // Respawn the fruits if enough time has passed
    for (unsigned int i = 0; i < game->n_fruits; ++i)
    {
        Fruit* fruit = game->fruits[i];
        if (!fruit->is_alive && time_diff_ms(fruit->eaten_time, now) > 2000)
            fruit_respawn(game, fruit);
    }
}

int main (int argc, char* argv[])
{
    // Change to the correct working directory
    change_to_exec_dir(argv[0]);

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
    // This magic formula apparently works, yay for no if else statement
    game->max_players = floor(((float)n_empty + 2.0) / 4.0);

    // Thread for handling incoming client connections
    pthread_t connect_to_clients_thread;
    pthread_create(&connect_to_clients_thread, NULL, connect_to_clients, (void*)game);

    // Create the SDL window
    create_board_window(board_get_size_x(game->board), board_get_size_y(game->board));

    // Draw the board, just the tiles
    draw_bricks(game);
    // Poll and draw loop
    SDL_Event event;
    int quit = 0;
    while (!quit){
        struct timespec frame_begin;
        clock_gettime(CLOCK_MONOTONIC, &frame_begin);

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
        
        // Run a game tick
        game_update(game);

        // Update the clients (if there are any)
        if (game->n_players)
        {
            send_to_all_clients(game, MESSAGE_BOARD, NULL);
            send_to_all_clients(game, MESSAGE_PLAYER_LIST, NULL);
            send_to_all_clients(game, MESSAGE_FRUIT_LIST, NULL);
            // Scoreboard is only sent every 60s
            static struct timespec last_scoreboard_time;
            if (last_scoreboard_time.tv_sec == 0)   // Static variables are thankfuly init'd as 0
                clock_gettime(CLOCK_MONOTONIC, &last_scoreboard_time);
            struct timespec now;
            clock_gettime(CLOCK_MONOTONIC, &now);
            if (time_diff_ms(last_scoreboard_time, now) > 60000)
            {
                send_to_all_clients(game, MESSAGE_PRINT_SCOREBOARD, NULL);
                clock_gettime(CLOCK_MONOTONIC, &last_scoreboard_time);
            }
        }

        // Clear the board to render over
        clear_board(board_get_size_x(game->board), board_get_size_y(game->board));
        draw_bricks(game);
        draw_players(game);
        draw_fruit(game);
        render_board();

        // ~ 60Hz server
        struct timespec frame_end;
        clock_gettime(CLOCK_MONOTONIC, &frame_end);
        SDL_Delay(16 /*- time_diff_ms(frame_begin, frame_end)*/);
	}

    pthread_kill(connect_to_clients_thread, SIGUSR1);    
    pthread_join(connect_to_clients_thread, NULL);

    return 0;
}