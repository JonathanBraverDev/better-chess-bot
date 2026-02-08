#include "Evaluation.h"
#include "Evaluation structure.h"

int Evaluator::evaluate(const Position& pos) {
    int ownScore = 0;
    int opponentScore = 0;

    ownScore += pos.getOwnPieces(PieceType::PAWN).countSetBits() * CENTIPAWNS.pawn_value;
    ownScore += pos.getOwnPieces(PieceType::KNIGHT).countSetBits() * CENTIPAWNS.knight_value;
    ownScore += pos.getOwnPieces(PieceType::BISHOP).countSetBits() * CENTIPAWNS.bishop_value;
    ownScore += pos.getOwnPieces(PieceType::ROOK).countSetBits() * CENTIPAWNS.rook_value;
    ownScore += pos.getOwnPieces(PieceType::QUEEN).countSetBits() * CENTIPAWNS.queen_value;

    opponentScore += pos.getOpponentPieces(PieceType::PAWN).countSetBits() * CENTIPAWNS.pawn_value;
    opponentScore += pos.getOpponentPieces(PieceType::KNIGHT).countSetBits() * CENTIPAWNS.knight_value;
    opponentScore += pos.getOpponentPieces(PieceType::BISHOP).countSetBits() * CENTIPAWNS.bishop_value;
    opponentScore += pos.getOpponentPieces(PieceType::ROOK).countSetBits() * CENTIPAWNS.rook_value;
    opponentScore += pos.getOpponentPieces(PieceType::QUEEN).countSetBits() * CENTIPAWNS.queen_value;

    return ownScore - opponentScore;
}
