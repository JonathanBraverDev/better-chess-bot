#pragma once

#include <vector>

#include "Board structure.h"

// constexpr save the function call so they're faster than regular function
// general bit manipulation
inline B64 set_bit(B64 board, int bit) { return (board |= (1ULL << bit)); } // shift 1 to position and set to OR
inline bool get_bit(B64 board, int bit) { return (board & (1ULL << bit)); } // shift 1 to position and use AND as a boolean check on the bit
inline B64 clear_bit(B64 board, int bit) { return (board &= ~(1ULL << bit)); } // shift 1 to position, INVERT and set to AND
inline B64 lowestBitBoard(B64 board) { return (board & -board); } // AND with 2's compliment to get the lowest bit (very cool)
inline B64 removeLowestBit(B64 board) { return (board & board - 1); } // AND after -1, garantees removal of exactly one lowest bit

// big finders
uint8_t lowestBitIndex64(B64 board);
uint8_t lowestBitIndex64_s(B64 board);
int count_bits64(B64 board);

// piece handling
void extract_pieces(B64 board, std::vector<B64>& pieces);

// piece movment assists, with bound protections
inline B64 up(B64 board) { return board << BOARD_SIDE; } // shifts out of the board would just be 0 anyway
inline B64 down(B64 board) { return board >> BOARD_SIDE; }
inline B64 left(B64 board) { return (board & ~COLUMN_A) << 1; }
inline B64 right(B64 board) { return (board & ~COLUMN_H) >> 1; }
inline B64 up_left(B64 board) { return (board & ~COLUMN_A) << (BOARD_SIDE + 1); }
inline B64 up_right(B64 board) { return (board & ~COLUMN_H) << (BOARD_SIDE - 1); }
inline B64 down_left(B64 board) { return (board & ~COLUMN_A) >> (BOARD_SIDE - 1); }
inline B64 down_right(B64 board) { return (board & ~COLUMN_H) >> (BOARD_SIDE + 1); }

// more complex sliding movemnt, each of these returns a board with all possible destinations
B64 slide(B64(*direction)(B64), const B64 all_pieces, B64 piece);
// aliases for the slide function
inline B64 slide_up(const B64 all_pieces, B64 piece) { return slide(&up, all_pieces, piece); };
inline B64 slide_down(const B64 all_pieces, B64 piece) { return slide(&down, all_pieces, piece); };
inline B64 slide_left(const B64 all_pieces, B64 piece) { return slide(&left, all_pieces, piece); };
inline B64 slide_right(const B64 all_pieces, B64 piece) { return slide(&right, all_pieces, piece); };
inline B64 slide_up_left(const B64 all_pieces, B64 piece) { return slide(&up_left, all_pieces, piece); };
inline B64 slide_up_right(const B64 all_pieces, B64 piece) { return slide(&up_right, all_pieces, piece); };
inline B64 slide_down_left(const B64 all_pieces, B64 piece) { return slide(&down_left, all_pieces, piece); };
inline B64 slide_down_right(const B64 all_pieces, B64 piece) { return slide(&down_right, all_pieces, piece); };