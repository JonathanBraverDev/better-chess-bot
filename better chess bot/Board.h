#pragma once

#include "Bitboard.h"
#include "Move.h"

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
    Bitboard black_king;
    Bitboard special_move_rigths; // en passant AND castle rights for both sides, they can't overlap anyway
public:
    void makeMove(Move move);
};
