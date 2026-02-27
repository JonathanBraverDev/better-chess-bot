#include "Search.h"
#include "Evaluation.h"
#include "EvaluationStructure.h"
#include <algorithm>
#include <limits>

Move Search::bestRootMove;

Move Search::search(Position& pos, int depth) {
    bestRootMove = Move(); // Reset for safety
    negamax(pos, depth, depth, -std::numeric_limits<int>::max(), std::numeric_limits<int>::max());
    return bestRootMove;
}

int Search::negamax(Position& pos, int depth, int maxDepth, int alpha, int beta) {
    
    // Evaluate at target depth
    if (depth == 0) {
        return Evaluator::evaluate(pos);
    }

    // Generate moves
    MoveList legalMoves = pos.getLegalMoves();

    // Resolve static end game
    if (legalMoves.empty()) {
        if (pos.isInCheck()) {
            return -WIN_VALUE + (maxDepth - depth); // Prefer faster mates
        } else {
            return DRAW_VALUE; // Stalemate
        }
    }

    // Sort moves descending (see encoding for exact ordering priority)
    std::sort(legalMoves.begin(), legalMoves.end(), std::greater<Move>());

    int bestScore = -std::numeric_limits<int>::max();

    for (const Move& move : legalMoves) {
        pos.makeMove(move);

        int score = -negamax(pos, depth - 1, maxDepth, -beta, -alpha);
        
        pos.undoMove(move);

        if (score > bestScore) {
            bestScore = score;
            bestRootMove = move;
        }

        if (score > alpha) {
            alpha = score;
        }

        if (score >= beta) {
            break; // Pruning
        }
    }

    return bestScore;
}
