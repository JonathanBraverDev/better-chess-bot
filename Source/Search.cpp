#include "Search.h"
#include "Evaluation.h"
#include "EvaluationStructure.h"
#include <algorithm>
#include <limits>

Move Search::search(Position &pos, int depth) {
  SearchResult result =
      negamax(pos, depth, depth, -std::numeric_limits<int>::max(),
              std::numeric_limits<int>::max());
  return result.bestMove;
}

SearchResult Search::negamax(Position &pos, int depth, int maxDepth, int alpha,
                             int beta) {

  // Evaluate at target depth
  if (depth == 0) {
    return {Evaluator::evaluate(pos), Move()};
  }

  // Generate moves
  MoveList legalMoves = pos.getLegalMoves();

  // Resolve static end game
  if (legalMoves.empty()) {
    if (pos.isInCheck()) {
      return {-WIN_VALUE + (maxDepth - depth), Move()}; // Prefer faster mates
    } else {
      return {DRAW_VALUE, Move()}; // Stalemate
    }
  }

  // Sort moves descending (see encoding for exact ordering priority)
  std::sort(legalMoves.begin(), legalMoves.end(), std::greater<Move>());

  int bestScore = -std::numeric_limits<int>::max();
  Move bestMove;

  for (const Move &move : legalMoves) {
    pos.makeMove(move);

    SearchResult childResult = negamax(pos, depth - 1, maxDepth, -beta, -alpha);
    int score = -childResult.score;

    pos.undoMove(move);

    if (score > bestScore) {
      bestScore = score;
      bestMove = move;
    }

    if (score > alpha) {
      alpha = score;
    }

    if (score >= beta) {
      break; // Pruning
    }
  }

  return {bestScore, bestMove};
}
