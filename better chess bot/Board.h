#pragma once

#include <vector>
#include "Bitboard.h"
#include "Move.h"
#include "Enums.h"

class Board {
private:
    Bitboard white_pawns;
    Bitboard white_knights;
    Bitboard white_bishops;
    Bitboard white_rooks;
    Bitboard white_queens;
    Bitboard white_king; // no 's' this time, budget cuts
    Bitboard black_pawns;
    Bitboard black_knights;
    Bitboard black_bishops;
    Bitboard black_rooks;
    Bitboard black_queens;
    Bitboard black_king; // I swear it's not just discrimination

    // moves the pieces could make
    std::vector<Move> getPotentialMoves(Color color);

public:
    void makeMove(Move move);

    Bitboard getPieces(Color color, PieceType type) const;
    Piece getPieceAtIndex(int index) const;
    bool validate() const;

    // moves that can be legally played
    std::vector<Move> getLegalMoves(Color color);
};
