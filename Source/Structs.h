#pragma once

#include "Enums.h"

struct Piece {
    Color color;
    PieceType type;
};

struct DirectionCheck {
    int shiftAmount;
    B64 boundCheck;
};
