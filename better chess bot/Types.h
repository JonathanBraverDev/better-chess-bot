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