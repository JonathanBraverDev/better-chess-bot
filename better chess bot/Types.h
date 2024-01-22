#pragma once

#include <cstdint>
#include "Enums.h"

typedef uint64_t B64; // 64 bit board
typedef uint32_t BitMove; // bit-encoded move format
typedef uint8_t BitRights; // minimalistic tracing of special move rights

struct Piece {
    Color color;
    PieceType type;
};

struct PrecomputedMoves {
    B64 king_moves[64];
    B64 knight_moves[64];
    B64 pawn_moves[64 * 2];
    B64 pawn_attacks[64 * 2];
};