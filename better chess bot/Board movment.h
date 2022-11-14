#pragma once

#include "Board structure.h"

// piece movment assists, with bound protections
constexpr B64 up(B64 board) { return board << 8; } // shifts out of the board would just be 0 anyway
constexpr B64 down(B64 board) { return board >> 8; }
constexpr B64 left(B64 board) { return (board & ~COLUMN_A) << 1; }
constexpr B64 right(B64 board) { return (board & ~COLUMN_H) >> 1; }
constexpr B64 up_left(B64 board) { return (board & ~COLUMN_A) << 9; }
constexpr B64 up_right(B64 board) { return (board & ~COLUMN_H) << 7; }
constexpr B64 down_left(B64 board) { return (board & ~COLUMN_A) >> 7; }
constexpr B64 down_right(B64 board) { return (board & ~COLUMN_H) >> 9; }

// more complex sliding movemnt, each of these returns a board with all possible destinations
void slide(B64(*direction)(B64), B64& same_color, B64& diff_color, B64& moves, B64 piece);
// aliases for the slide function
void slide_up(B64& same_color, B64& diff_color, B64& moves, B64 piece);
void slide_down(B64& same_color, B64& diff_color, B64& moves, B64 piece);
void slide_left(B64& same_color, B64& diff_color, B64& moves, B64 piece);
void slide_right(B64& same_color, B64& diff_color, B64& moves, B64 piece);
void slide_up_left(B64& same_color, B64& diff_color, B64& moves, B64 piece);
void slide_up_right(B64& same_color, B64& diff_color, B64& moves, B64 piece);
void slide_down_left(B64& same_color, B64& diff_color, B64& moves, B64 piece);
void slide_down_right(B64& same_color, B64& diff_color, B64& moves, B64 piece);
