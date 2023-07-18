#pragma once

#include <vector>
#include <iostream>
#include <cmath>

#include "Board structure.h"

// important note: all movment functions are true to the view printed by this function
//				   all bitshifts are flipped
void visualize_board(const B64 board);

B64 rotate180(B64 board);

// general bit manipulation
inline void set_bit(B64& board, int bit) {  (board |= (1ULL << bit)); } // shift 1 to position and set to OR
inline bool get_bit(const B64 board, int bit) { return (board & (1ULL << bit)); } // shift 1 to position and use AND as a boolean check on the bit
inline void clear_bit(B64& board, int bit) { (board &= ~(1ULL << bit)); } // shift 1 to position, INVERT and set to AND
inline void flip_bit(B64& board, int bit) { (board ^= (1ULL << bit)); } // shift 1 to position and set to XOR
inline B64 lowest_bit_board(B64 board) { return (board & -board); } // AND with 2's compliment to get the lowest bit (very cool)
inline B64 remove_lowest_bit(B64 board) { return (board & (board - 1)); } // AND after -1, garantees removal of exactly one lowest bit
// alternatives that take 2 boards
inline void set_bits(B64& board, B64 bit_board) { (board |= bit_board); }
inline B64 copy_bits(B64& board, B64 bit_board) { (board & bit_board); }
inline void clear_bits(B64& board, B64 bit_board) { (board &= ~bit_board); }
inline void flip_bits(B64& board, B64 bit_board) { (board ^= bit_board); }

inline float get_distance(const int x1, const int y1, const int x2, const int y2) { return std::hypotf(x1 - x2, y1 - y2); }
float get_distance(B64& bit_board1, B64& bit_board2);

// big finders
int lowest_bit_index(const B64 board);
int lowest_single_bit_index(const B64 board);
void lowest_single_cords(const B64 board, int& x, int& y);
int count_bits64(B64 board);
inline B64 bit_board_from_index(const int index) { return (1ULL << index); }

inline B64 lower_than_bit(const B64 board) { return (1ULL << lowest_single_bit_index(board)) - 1; }
inline B64 higher_than_bit(const B64 board) { return (-1ULL << lowest_single_bit_index(board)); }
inline B64 lower_than_bit_index(const int index) { return (1ULL << index) - 1; }
inline B64 higher_than_bit_index(const int index) { return (-1ULL << index); }

// piece handling
void seperate_bits(B64 board, std::vector<B64>& pieces);

// piece movment assists, with bound protections
inline B64 up(B64 board) { return board << BOARD_SIZE; } // shifts out of the board would just be 0 anyway
inline B64 down(B64 board) { return board >> BOARD_SIZE; }
inline B64 left(B64 board) { return (board & COLUMN_H_INV) >> 1; }
inline B64 right(B64 board) { return (board & COLUMN_A_INV) << 1; }
inline B64 up_left(B64 board) { return (board & COLUMN_H_INV) << BOARD_SIDE_SUB1; }
inline B64 up_right(B64 board) { return (board & COLUMN_A_INV) << BOARD_SIDE_ADD1; }
inline B64 down_left(B64 board) { return (board & COLUMN_H_INV) >> BOARD_SIDE_ADD1; }
inline B64 down_right(B64 board) { return (board & COLUMN_A_INV) >> BOARD_SIDE_SUB1; }
// note: all directional names are are according to the output of "visualize_board"

// more complex sliding movemnt, each of these returns a board with all possible destinations
B64 slide(B64(*const direction)(B64), const B64 all_pieces, B64 piece);
// doesnt add the final move
B64 safe_slide(B64(* const direction)(B64), const B64 blockers, B64 piece);

// aliases for the slide function
inline B64 slide_up(const B64 all_pieces, B64 piece) { return slide(&up, all_pieces, piece); };
inline B64 slide_down(const B64 all_pieces, B64 piece) { return slide(&down, all_pieces, piece); };
inline B64 slide_left(const B64 all_pieces, B64 piece) { return slide(&left, all_pieces, piece); };
inline B64 slide_right(const B64 all_pieces, B64 piece) { return slide(&right, all_pieces, piece); };
inline B64 slide_up_left(const B64 all_pieces, B64 piece) { return slide(&up_left, all_pieces, piece); };
inline B64 slide_up_right(const B64 all_pieces, B64 piece) { return slide(&up_right, all_pieces, piece); };
inline B64 slide_down_left(const B64 all_pieces, B64 piece) { return slide(&down_left, all_pieces, piece); };
inline B64 slide_down_right(const B64 all_pieces, B64 piece) { return slide(&down_right, all_pieces, piece); };

// pretty complex functions
B64(*get_direction(const B64 start, const B64 end))(B64); // apparantly that's how you write that signatue
B64 get_connecting_tiles(B64 start, const B64 end);