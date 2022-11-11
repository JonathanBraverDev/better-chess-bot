#pragma once

#include <cstdint>
#include <vcruntime_string.h>

typedef uint64_t B64; // 64 bit board

// EVERYTHING needed for quick operations on the position
struct BoardPosition {
    // everything here is a 'bitmap of...'
    B64 white;
    B64 white_pawns;
    B64 white_bishops;
    B64 white_knights;
    B64 white_rooks;
    B64 white_queens;
    B64 white_king; // no s this time, budget cuts
    B64 black;
    B64 black_pawns;
    B64 black_bishops;
    B64 black_knights;
    B64 black_rooks;
    B64 black_queens;
    B64 black_king;
    B64 special_move_rigths; // en passant AND caslte right bot both sides, data cant overlap anyway
};

// info on gamestate
struct GameState { // split off from positiopn to allow memcmp (both the timer and pointer whould interefere)
    int turn; // idk why i need this actually, but why not, it's kinda cute
    BoardPosition position;
    int draw_timer; // used to count moves untill the forced draw
    GameState* previous_state; // used for 3 time repetition checks and move history
};

inline bool operator==(const BoardPosition& lhs, const BoardPosition& rhs) {
    return memcmp(&lhs, &rhs, sizeof(BoardPosition));
}

// useful for out of bounds detection
const B64 COLUMN_A = 0x8080808080808080ULL;
const B64 COLUMN_B = 0x4040404040404040ULL;
const B64 COLUMN_AB = COLUMN_A | COLUMN_B;
const B64 COLUMB_G = 0x0202020202020202ULL;
const B64 COLUMN_H = 0x0101010101010101ULL;
const B64 COLUMN_GH = COLUMB_G | COLUMN_H;
// used for pawn first move/promotion
const B64 ROW_1 = 0xFF00000000000000ULL;
const B64 ROW_2 = 0x00FF000000000000ULL;
//const B64 ROW_3 = 0x0000FF0000000000ULL;
//const B64 ROW_4 = 0x000000FF00000000ULL;
//const B64 ROW_5 = 0x00000000FF000000ULL;
//const B64 ROW_6 = 0x0000000000FF0000ULL;
const B64 ROW_7 = 0x000000000000FF00ULL;
const B64 ROW_8 = 0x00000000000000FFULL;

// constexpr save the function call so they're faster than regular function
// general bit manipulation
constexpr B64 set_bit(B64 board, int bit) { return (board |= (1ULL << bit)); } // shift 1 to position and set to OR
constexpr bool get_bit(B64 board, int bit) { return (board & (1ULL << bit)); } // shift 1 to position and use AND as a boolean check on the bit
constexpr B64 clear_bit(B64 board, int bit) { return (board &= ~(1ULL << bit)); } // shift 1 to position, INVERT and set to AND

// loads index of first bit found to the bit variable
# define find_first_bit(board, bit) _BitScanForward64(&bit, board) // not sure if ill use this anywhere