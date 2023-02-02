#pragma once

#include "Board structure.h"

enum PieceType {
    NONE,
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

inline bool Is_castle(Move move) { return move.type == CASTLE_SHORT || move.type == CASTLE_LONG; };
inline bool Is_capture(Move move) { return move.type == CAPTURE || move.type == PROMOTION_CAPTURE; };
inline bool Is_promotion(Move move) { return move.type == PROMOTION || move.type == PROMOTION_CAPTURE; };


BoardPosition Make_move(BoardPosition position, Move move);
BoardPosition Undo_move(BoardPosition position, Move move);

void Toggle_castle(BoardPosition& position, const Move& move);
void Move_piece(BoardPosition& position, const Move& move, bool reverse = false);
void Toggle_captured(BoardPosition& position, const Move& move);
void Toggle_promotion(BoardPosition& position, const Move& move);