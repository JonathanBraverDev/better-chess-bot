#pragma once
#include "Position.h"

class Evaluator {
public:
    // Static evaluation of the current position from the perspective of the side to move.
    static int evaluate(const Position& pos);
};
