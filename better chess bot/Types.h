#pragma once

#include <cstdint>
#include "Enums.h"

typedef uint64_t B64; // 64 bit board
typedef uint32_t BitMove; // bit-encoded move format
typedef uint8_t BitRights; // minimalistic traching of special moverights

struct Piece {
    Color color;
    PieceType type;
};

// will be "promoted" to a class later
struct Replay {
    Color winner;
    GameOverCause endType;
    GameState finalState;
};