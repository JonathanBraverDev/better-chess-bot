#pragma once

#include <vcruntime_string.h>

#include "Board structure.h"
#include "Piece structure.h"

// the position thorough the eyes of the player who's turn it is
struct SidedPosition {
    // everything here is a 'bitmap of...'
    B64 own_pawns;
    B64 own_knights;
    B64 own_bishops;
    B64 own_rooks;
    B64 own_queens;
    B64 own_king; // no 's' this time, budget cuts
    B64 opponent_pawns;
    B64 opponent_knights;
    B64 opponent_bishops;
    B64 opponent_rooks;
    B64 opponent_queens;
    B64 opponent_king;
    B64 special_move_rigths; // en passant AND castle right bot both sides, data cant overlap anyway
    bool is_white;
};

inline B64 own_pieces(const SidedPosition sided_position) {
    return (sided_position.own_pawns | sided_position.own_knights | sided_position.own_bishops |
            sided_position.own_rooks | sided_position.own_queens | sided_position.own_king);
}

inline B64 opponent_pieces(const SidedPosition sided_position) {
    return (sided_position.opponent_pawns | sided_position.opponent_knights | sided_position.opponent_bishops |
            sided_position.opponent_rooks | sided_position.opponent_queens | sided_position.opponent_king);
}

inline B64 all_pieces(const SidedPosition sided_position) { return (own_pieces(sided_position) | opponent_pieces(sided_position)); }

void switch_sides(SidedPosition& sided_position);
B64* own_piece_board(SidedPosition& sided_position, const PieceType type);

// info on gamestate
struct GameState { // split off from positiopn to allow memcmp (both the timer and pointer whould interefere)
    int turn; // idk why i need this actually, but why not, it's kinda cute
    int draw_timer; // used to count moves untill the forced draw
    SidedPosition sided_position;
    GameState* previous_state; // used for 3 time repetition checks and move history
};

inline bool operator==(const SidedPosition& lhs, const SidedPosition& rhs) {
    return memcmp(&lhs, &rhs, sizeof(SidedPosition));
}

inline B64 own_pieces(const GameState state) { return own_pieces(state.sided_position); }
inline B64 opponent_pieces(const GameState state) { return opponent_pieces(state.sided_position); }
inline B64 all_pieces(const GameState state) { return all_pieces(state.sided_position); }