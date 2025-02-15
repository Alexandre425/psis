#include <SDL2/SDL.h>

void get_board_place(int mouse_x, int mouse_y, int * board_x, int *board_y);
int create_board_window(int dim_x, int dim_y);
void close_board_windows();
void paint_pacman(int  board_x, int board_y , int r, int g, int b, int powered_up);
void paint_monster(int  board_x, int board_y , int r, int g, int b);
void paint_lemon(int  board_x, int board_y );
void paint_cherry(int  board_x, int board_y);
void paint_brick(int  board_x, int board_y );
void clear_place(int  board_x, int board_y);

// Renders the board
void render_board(void);
// Clears all the tiles in the board
void clear_board(int size_x, int size_y);