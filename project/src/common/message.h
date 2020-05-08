#pragma once

#include <stdlib.h>

#include "utilities.h"
#include "pacman.h"

// Implements functions that serialize data to be sent over the network
//
//
// Packing an object does the following:
//  - Specifies their data types (int -> int32_t, etc) (to deal with byte-sizes of intrinsic types)
//  - Converts them to network format (htnol, etc) (to deal with endianness)
//  - Puts them in packed structs (to deal with padding)
//
// Unpacking an object does the exact opposite

// Precedes every message, tells the server/client how to interpret the following bytes
enum MessageType {message_terminator = -1, message_color, message_board, message_pacman, message_monster};

// Network format of a color
typedef struct _packed_color packed_Color;
// Creates a packed color struct
packed_Color* message_packed_color_create(void);
// Destroys a packed color struct
void message_packed_color_destroy(packed_Color* p_color);
// Packs "color" and puts it in "p_color"
// Returns the size in bytes
size_t message_pack_color(Color* color, packed_Color* p_color);

// Network format of a fruit
typedef struct _packed_fruit packed_Fruit;