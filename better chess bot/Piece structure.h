#pragma once

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

struct Piece {
    PieceType type;
    PlayerColor color;
};
