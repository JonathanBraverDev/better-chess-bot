#pragma once
#include "Position.h"

class Evaluator {
private:
    static int evaluateMaterial(const Position& pos, Color color);

public:
    // Static evaluation of the current position from the perspective of the side to move.
    static int evaluate(const Position& pos);
};
