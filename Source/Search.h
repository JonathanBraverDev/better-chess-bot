#pragma once
#include "Position.h"
#include "Move.h"

struct SearchResult {
    int score;
    Move bestMove;
};

class Search {
public:
    // Returns the best move found for the current position at the given depth.
    static Move search(Position& pos, int depth);

private:
    // Returns score and best move relative to side to move
    static SearchResult negamax(Position& pos, int depth, int maxDepth, int alpha, int beta);
};
