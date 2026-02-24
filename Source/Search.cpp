#include "Search.h"
#include "Evaluation.h"
#include "EvaluationStructure.h"
#include <algorithm>
#include <limits>

Move Search::search(Position& pos, int depth) {
    MoveList legalMoves = pos.getLegalMoves();
    if (legalMoves.empty()) {
        return Move(); // Return invalid move if no moves available
    }

    // Sort moves descending (see encoding for exact ordering priority)
    std::sort(legalMoves.begin(), legalMoves.end(), std::greater<Move>());

    Move bestMove;
    int bestScore = -std::numeric_limits<int>::max();
    int alpha = -std::numeric_limits<int>::max();
    int beta = std::numeric_limits<int>::max();

    for (const Move& move : legalMoves) {
        pos.makeMove(move);
        
        // Negamax: score = -negamax(child)
        int score = -negamax(pos, depth - 1, -beta, -alpha);
        
        pos.undoMove(move);

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

int Search::negamax(Position& pos, int depth, int alpha, int beta) {
    
    // Evaluate at target depth
    if (depth == 0) {
        return Evaluator::evaluate(pos);
    }

    // Generate moves
    MoveList legalMoves = pos.getLegalMoves();

    // Resolve static end game
    if (legalMoves.empty()) {
        if (pos.isInCheck()) {
            return -WIN_VALUE + (100 - depth); // Prefer faster mates
        } else {
            return DRAW_VALUE; // Stalemate
        }
    }

    // Sort moves descending (MVV-LVA, Checks, Promotions first)
    std::sort(legalMoves.begin(), legalMoves.end(), std::greater<Move>());

    int bestScore = -std::numeric_limits<int>::max();

    for (const Move& move : legalMoves) {
        pos.makeMove(move);

        int score = -negamax(pos, depth - 1, -beta, -alpha);
        
        pos.undoMove(move);

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
