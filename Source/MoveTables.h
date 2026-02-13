#pragma once

#include "Bitboard.h"

class MoveTables {
public:
    static Bitboard king_moves[64];
    static Bitboard knight_moves[64];
    static Bitboard pawn_moves[64 * 2];
    static Bitboard pawn_attacks[64 * 2];

    static void initialize();

private:
    static void prepareKingMoves();
    static void prepareKnightMoves();
    static void prepareWhitePawnMoves();
    static void prepareBlackPawnMoves();
};
