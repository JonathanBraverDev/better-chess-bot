#include "MoveTables.h"

Bitboard MoveTables::king_moves[64];
Bitboard MoveTables::knight_moves[64];
Bitboard MoveTables::pawn_moves[64 * 2];
Bitboard MoveTables::pawn_attacks[64 * 2];

void MoveTables::initialize() {
    prepareKingMoves();
    prepareKnightMoves();
    prepareWhitePawnMoves();
    prepareBlackPawnMoves();
}

void MoveTables::prepareKingMoves() {
    Bitboard king = Bitboard(1ULL);
    for (size_t i = 0; i < 64; i++) {
        king_moves[i] = combineBoards(
            king.look(Direction::UP), king.look(Direction::DOWN),
            king.look(Direction::LEFT), king.look(Direction::RIGHT),
            king.look(Direction::UP_LEFT), king.look(Direction::UP_RIGHT),
            king.look(Direction::DOWN_LEFT), king.look(Direction::DOWN_RIGHT));

        king.nextTile();
    }
}

void MoveTables::prepareKnightMoves() {
    Bitboard knight = Bitboard(1ULL);
    for (size_t i = 0; i < 64; i++) {
        knight_moves[i] =
            combineBoards(knight.look(Direction::KNIGHT_UP_LEFT),
                knight.look(Direction::KNIGHT_UP_RIGHT),
                knight.look(Direction::KNIGHT_DOWN_LEFT),
                knight.look(Direction::KNIGHT_DOWN_RIGHT),
                knight.look(Direction::KNIGHT_LEFT_UP),
                knight.look(Direction::KNIGHT_LEFT_DOWN),
                knight.look(Direction::KNIGHT_RIGHT_UP),
                knight.look(Direction::KNIGHT_RIGHT_DOWN));
        knight.nextTile();
    }
}

void MoveTables::prepareWhitePawnMoves() {
    Bitboard white_pawn = Bitboard(1ULL);
    for (size_t i = 0; i < 64 * 2; i += 2) {
        pawn_moves[i] = white_pawn.look(Direction::UP);

        pawn_attacks[i] =
            combineBoards(white_pawn.look(Direction::UP_LEFT),
                white_pawn.look(Direction::UP_RIGHT));

        white_pawn.nextTile();
    }
}

void MoveTables::prepareBlackPawnMoves() {
    Bitboard black_pawn = Bitboard(1ULL);
    for (size_t i = 1; i < 64 * 2; i += 2) {
        pawn_moves[i] = black_pawn.look(Direction::DOWN);

        pawn_attacks[i] =
            combineBoards(black_pawn.look(Direction::DOWN_LEFT),
                black_pawn.look(Direction::DOWN_RIGHT));

        black_pawn.nextTile();
    }
}
