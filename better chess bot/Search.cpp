#include "Search.h"
#include "Evaluation.h"
#include "Evaluation structure.h"
#include <vector>
#include <algorithm>
#include <limits>

Move Search::search(const Position& pos, int depth) {
    std::vector<Move> legalMoves = pos.getLegalMoves();
    if (legalMoves.empty()) {
        return Move(); // Return invalid move if no moves available
    }

    Move bestMove;
    int bestScore = -std::numeric_limits<int>::max();
    int alpha = -std::numeric_limits<int>::max();
    int beta = std::numeric_limits<int>::max();

    for (const Move& move : legalMoves) {
        Position childPos = pos;
        childPos.makeMove(move);
        
        // Negamax: score = -negamax(child)
        int score = -negamax(childPos, depth - 1, -beta, -alpha);

        if (score > bestScore) {
            bestScore = score;
            bestMove = move;
        }
        
        if (score > alpha) {
            alpha = score;
        }
    }

    return bestMove;
}

int Search::negamax(const Position& pos, int depth, int alpha, int beta) {
    
    // Evaluate at target depth
    if (depth == 0) {
        return Evaluator::evaluate(pos);
    }

    // Generate moves
    std::vector<Move> legalMoves = pos.getLegalMoves();

    // Resolve static end game
    if (legalMoves.empty()) {
        if (pos.isInCheck()) {
            return -WIN_VALUE + (100 - depth); // Prefer faster mates
        } else {
            return DRAW_VALUE; // Stalemate
        }
    }

    int bestScore = -std::numeric_limits<int>::max();

    for (const Move& move : legalMoves) {
        Position childPos = pos;
        childPos.makeMove(move);

        int score = -negamax(childPos, depth - 1, -beta, -alpha);

        if (score > bestScore) {
            bestScore = score;
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
