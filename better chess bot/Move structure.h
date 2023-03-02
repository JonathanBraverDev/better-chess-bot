#pragma once

#include "Board structure.h"
#include "Position structure.h"
#include "Piece structure.h"

enum MoveType {
    NORMAL,
    CAPTURE,
    CASTLE_SHORT,
    CASTLE_LONG,
    PROMOTION,
    PROMOTION_CAPTURE
};

struct Move {
    B64 origin;
    B64 destination; // castling rook here, offers full compaability with 960 chess, pround of that one
    Piece piece; // possibly add members directly here
    MoveType type;
    PieceType captured_type;
    PieceType promoted_type;
    B64 special_move_rigths; // avalible BEFORE the move, undoing this is such a headache i just gave up
    // do i really need to be able to undo moves at all?
};

inline bool is_white_player(const GameState& state) { return (state.turn % 2 == 0); };
inline Move invert_move(Move move) { 
    swap(move.origin, move.destination);
    return move; 
}