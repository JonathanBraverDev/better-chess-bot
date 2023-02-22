#pragma once

#include "Board structure.h"

enum PieceType {
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING,
    NONE
};

enum PlayerColor {
    WHITE,
    BLACK
};

enum MoveType {
    NORMAL,
    CAPTURE,
    CASTLE_SHORT,
    CASTLE_LONG,
    PROMOTION,
    PROMOTION_CAPTURE
};

struct Piece {
    PieceType type;
    PlayerColor color;
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

inline PlayerColor determine_player(const GameState& state) { return (state.turn % 2 == 0 ? WHITE : BLACK); };