#pragma once

#include "Board structure.h"

enum PieceType {
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING
};

enum Color {
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
    Color color;
};

struct Move {
    B64 origin;
    B64 destination; // castling rook here, offers full compaability with 960 chess, pround of that one
    Piece piece; // possibly add members directly here
    MoveType type;
    PieceType captured_type;
    PieceType promoted_type;
};

Move Invert_move(Move move);

BoardPosition Make_move(BoardPosition position, Move move);
BoardPosition Undo_move(BoardPosition position, Move move);