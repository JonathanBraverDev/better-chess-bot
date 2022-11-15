#pragma once

#include <cstdint>

#include "Board structure.h"

// big finders
uint8_t lowestBitIndex(uint32_t v);
uint8_t lowestBitIndex32_2(uint64_t v);
uint8_t lowestBitIndex64(uint64_t v);
inline B64 lowestBitBoard(B64 board) { return (board & -board); }

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
B64 slide(B64(*direction)(B64), const B64 all_pieces, B64 piece);
// aliases for the slide function
B64 slide_up(const B64 all_pieces, B64 piece);
B64 slide_down(const B64 all_pieces, B64 piece);
B64 slide_left(const B64 all_pieces, B64 piece);
B64 slide_right(const B64 all_pieces, B64 piece);
B64 slide_up_left(const B64 all_pieces, B64 piece);
B64 slide_up_right(const B64 all_pieces, B64 piece);
B64 slide_down_left(const B64 all_pieces, B64 piece);
B64 slide_down_right(const B64 all_pieces, B64 piece);