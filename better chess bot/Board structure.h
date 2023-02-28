#pragma once

#include <vcruntime_string.h>

#include "Tile structure.h"

// EVERYTHING needed for quick operations on the position
struct BoardPosition {
    // everything here is a 'bitmap of...'
    B64 white_pawns;
    B64 white_knights;
    B64 white_bishops;
    B64 white_rooks;
    B64 white_queens;
    B64 white_king; // no 's' this time, budget cuts
    B64 black_pawns;
    B64 black_knights;
    B64 black_bishops;
    B64 black_rooks;
    B64 black_queens;
    B64 black_king;
    B64 special_move_rigths; // en passant AND castle right bot both sides, data cant overlap anyway
};

inline B64 all_white_pieces(const BoardPosition position) {
    return (position.white_pawns | position.white_knights | position.white_bishops | position.white_rooks | position.white_queens | position.white_king);
}

inline B64 all_black_pieces(const BoardPosition position) {
    return (position.black_pawns | position.black_knights | position.black_bishops | position.black_rooks | position.black_queens | position.black_king);
}

inline B64 all_pieces(const BoardPosition position) { return (all_white_pieces(position) | all_black_pieces(position)); }


// info on gamestate
struct GameState { // split off from positiopn to allow memcmp (both the timer and pointer whould interefere)
    int turn; // idk why i need this actually, but why not, it's kinda cute
    int draw_timer; // used to count moves untill the forced draw
    BoardPosition position;
    GameState* previous_state; // used for 3 time repetition checks and move history
};

inline bool operator==(const BoardPosition& lhs, const BoardPosition& rhs) {
    return memcmp(&lhs, &rhs, sizeof(BoardPosition));
}

inline B64 all_white_pieces(const GameState state) { return all_black_pieces(state.position); }
inline B64 all_black_pieces(const GameState state) { return all_black_pieces(state.position); }
inline B64 all_pieces(const GameState state) { return all_pieces(state.position); }

// constexpr > const, compiletime vs runtime evaluation, less runtime -> more better.
constexpr int BOARD_SIZE = 8;

// useful for out of bounds detection
constexpr B64 COLUMN_A = A1 | A2 | A3 | A4 | A5 | A6 | A7 | A8;
constexpr B64 COLUMN_B = B1 | B2 | B3 | B4 | B5 | B6 | B7 | B8;
constexpr B64 COLUMN_AB = COLUMN_A | COLUMN_B;
constexpr B64 COLUMN_C = C1 | C2 | C3 | C4 | C5 | C6 | C7 | C8;
constexpr B64 COLUMN_D = D1 | D2 | D3 | D4 | D5 | D6 | D7 | D8;
constexpr B64 COLUMN_E = E1 | E2 | E3 | E4 | E5 | E6 | E7 | E8;
constexpr B64 COLUMB_F = F1 | F2 | F3 | F4 | F5 | F6 | F7 | F8;
constexpr B64 COLUMB_G = G1 | G2 | G3 | G4 | G5 | G6 | G7 | G8;
constexpr B64 COLUMN_H = H1 | H2 | H3 | H4 | H5 | H6 | H7 | H8;
constexpr B64 COLUMN_GH = COLUMB_G | COLUMN_H;

// used for pawn first move/promotion
constexpr B64 ROW_8 = A8 | B8 | C8 | D8 | E8 | F8 | G8 | H8;
constexpr B64 ROW_7 = A7 | B7 | C7 | D7 | E7 | F7 | G7 | H7;
constexpr B64 ROW_6 = A6 | B6 | C6 | D6 | E6 | F6 | G6 | H6;
constexpr B64 ROW_5 = A5 | B5 | C5 | D5 | E5 | F5 | G5 | H5;
constexpr B64 ROW_4 = A4 | B4 | C4 | D4 | E4 | F4 | G4 | H4;
constexpr B64 ROW_3 = A3 | B3 | C3 | D3 | E3 | F3 | G3 | H3;
constexpr B64 ROW_2 = A2 | B2 | C2 | D2 | E2 | F2 | G2 | H2;
constexpr B64 ROW_1 = A1 | B1 | C1 | D1 | E1 | F1 | G1 | H1;
constexpr B64 TILE_BLACK = 0xAAAAAAAAAAAAAAAAULL;
constexpr B64 TILE_WHITE = 0x5555555555555555ULL;

// programmatically needed splits of the board
constexpr B64 VOID_WHITE_CASTLE = ~ROW_1;
constexpr B64 VOID_BLACK_CASTLE = ~ROW_8;
constexpr B64 VOID_EN_PASSANT = ~ROW_3 | ~ROW_6;

// used mainly for board slides, defined here for clarity
constexpr int BOARD_SIDE_ADD1 = BOARD_SIZE + 1; // this honestly feel INCREDIBLY stupid but I WAS calculating that every single call so... yeah
constexpr int BOARD_SIDE_SUB1 = BOARD_SIZE - 1;
constexpr B64 COLUMN_A_INV = ~COLUMN_A;
constexpr B64 COLUMN_H_INV = ~COLUMN_H;