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

struct Piece {
    PieceType type;
    Color color;
};

struct Move {
    B64 origin;
    B64 destination;
    Piece piece;
};

Move Invert_move(Move move);

BoardPosition Make_move(BoardPosition position, Move move);
BoardPosition Undo_move(BoardPosition position, Move move);