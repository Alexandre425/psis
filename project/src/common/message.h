#pragma once

//

// Precedes every message, tells the server/client how to interpret the following bytes
enum MessageType {message_terminator = -1, message_color, message_board, message_pacman, message_monster};