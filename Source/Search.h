#pragma once
#include "Position.h"
#include "Move.h"

class Search {
public:
    // Returns the best move found for the current position at the given depth.
    static Move search(const Position& pos, int depth);

private:
    // Returns score relative to side to move
    static int negamax(const Position& pos, int depth, int alpha, int beta);
};
