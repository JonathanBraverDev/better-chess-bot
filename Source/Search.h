#pragma once
#include "Position.h"
#include "Move.h"

class Search {
public:
    // Returns the best move found for the current position at the given depth.
    static Move search(Position& pos, int depth);

private:
    static Move bestRootMove;
    
    // Returns score relative to side to move
    static int negamax(Position& pos, int depth, int maxDepth, int alpha, int beta);
};
