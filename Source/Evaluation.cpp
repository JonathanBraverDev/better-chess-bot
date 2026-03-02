#include "Evaluation.h"
#include "EvaluationStructure.h"

int Evaluator::evaluateMaterial(const Position &pos, Color color) {
  int score = 0;
  score +=
      pos.getPieces(color, PieceType::PAWN).countSetBits() * CENTIPAWNS.pawn;
  score += pos.getPieces(color, PieceType::KNIGHT).countSetBits() *
           CENTIPAWNS.knight;
  score += pos.getPieces(color, PieceType::BISHOP).countSetBits() *
           CENTIPAWNS.bishop;
  score +=
      pos.getPieces(color, PieceType::ROOK).countSetBits() * CENTIPAWNS.rook;
  score +=
      pos.getPieces(color, PieceType::QUEEN).countSetBits() * CENTIPAWNS.queen;
  return score;
}

int Evaluator::evaluate(const Position &pos) {
  int ownScore = evaluateMaterial(pos, pos.getCurrentColor());
  int opponentScore = evaluateMaterial(pos, pos.getOpponentColor());

  return ownScore - opponentScore;
}
