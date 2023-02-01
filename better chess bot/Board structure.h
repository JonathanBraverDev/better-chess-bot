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
    B64 white_king; // no 's' this time, budget cuts
    B64 black;
    B64 black_pawns;
    B64 black_bishops;
    B64 black_knights;
    B64 black_rooks;
    B64 black_queens;
    B64 black_king;
    B64 special_move_rigths; // en passant AND castle right bot both sides, data cant overlap anyway
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

const int BOARD_SIDE = 8;

// useful for out of bounds detection
const B64 COLUMN_A = 0x8080808080808080ULL;
const B64 COLUMN_B = 0x4040404040404040ULL;
const B64 COLUMN_AB = COLUMN_A | COLUMN_B;
const B64 COLUMN_C = 0x2020202020202020ULL;
const B64 COLUMN_D = 0x1010101010101010ULL;
const B64 COLUMB_F = 0x0404040404040404ULL;
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
const B64 TILE_BLACK = 0xAAAAAAAAAAAAAAAAULL;
const B64 TILE_WHITE = 0x5555555555555555ULL;