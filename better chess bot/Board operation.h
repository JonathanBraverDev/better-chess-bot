#pragma once

#include <cstdint>

#include "Board structure.h"

// big finders
uint8_t lowestBitIndex64(B64 board);
uint8_t lowestBitIndex64_s(B64 board);
inline B64 lowestBitBoard(B64 board) { return (board & -board); }

// piece movment assists, with bound protections
inline B64 up(B64 board) { return board << 8; } // shifts out of the board would just be 0 anyway
inline B64 down(B64 board) { return board >> 8; }
inline B64 left(B64 board) { return (board & ~COLUMN_A) << 1; }
inline B64 right(B64 board) { return (board & ~COLUMN_H) >> 1; }
inline B64 up_left(B64 board) { return (board & ~COLUMN_A) << 9; }
inline B64 up_right(B64 board) { return (board & ~COLUMN_H) << 7; }
inline B64 down_left(B64 board) { return (board & ~COLUMN_A) >> 7; }
inline B64 down_right(B64 board) { return (board & ~COLUMN_H) >> 9; }

// more complex sliding movemnt, each of these returns a board with all possible destinations
B64 slide(B64(*direction)(B64), const B64 all_pieces, B64 piece);
// aliases for the slide function
inline B64 slide_up(const B64 all_pieces, B64 piece) { return slide(&up, all_pieces, piece); };
inline B64 slide_down(const B64 all_pieces, B64 piece) { return slide(&up, all_pieces, piece); };
inline B64 slide_left(const B64 all_pieces, B64 piece) { return slide(&up, all_pieces, piece); };
inline B64 slide_right(const B64 all_pieces, B64 piece) { return slide(&up, all_pieces, piece); };
inline B64 slide_up_left(const B64 all_pieces, B64 piece) { return slide(&up, all_pieces, piece); };
inline B64 slide_up_right(const B64 all_pieces, B64 piece) { return slide(&up, all_pieces, piece); };
inline B64 slide_down_left(const B64 all_pieces, B64 piece) { return slide(&up, all_pieces, piece); };
inline B64 slide_down_right(const B64 all_pieces, B64 piece) { return slide(&up, all_pieces, piece); };