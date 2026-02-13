#pragma once

#include "Position.h"
#include <vector>

class MoveGenerator {
public:
    static std::vector<Move> getLegalMoves(const Position& pos);
    static bool isAttackedByAnyPattern(const Position& pos, Bitboard target, Bitboard blockers);

private:
   // Internal state for the generator (optional, or pass around)
   // We'll use static helper functions passing necessary context

   static void getPawnMoves(const Position& pos, std::vector<Move>& moves);
   static void getKnightMoves(const Position& pos, std::vector<Move>& moves);
   static void getBishopMoves(const Position& pos, std::vector<Move>& moves);
   static void getRookMoves(const Position& pos, std::vector<Move>& moves);
   static void getQueenMoves(const Position& pos, std::vector<Move>& moves);
   static void getKingMoves(const Position& pos, std::vector<Move>& moves);
   
   static void getSlidingPieceMoves(const Position& pos, std::vector<Move>& moves, PieceType pieceType);
   static void getCastlingMoves(const Position& pos, std::vector<Move>& moves, Bitboard king, Bitboard blockers, Move move_base);

   // Helpers
   static void checkAndAddPawnJump(const Position& pos, std::vector<Move>& moves, Bitboard step, Bitboard empty_tiles, Move move_base, Direction forward);
   static void checkAndAddEnPassant(const Position& pos, std::vector<Move>& moves, Bitboard possible_en_passant, int pawn_move_index, Move move_base);
   static void addPromotionMoves(const Position& pos, std::vector<Move>& moves, Bitboard step, Bitboard captures, Move move_base);
   static void addNormalPawnMoves(const Position& pos, std::vector<Move>& moves, Move base_move, Bitboard step, Bitboard captures);
   
   static void finalizeMoves(const Position& pos, std::vector<Move>& moves, Bitboard destinations, Move move_base);
   static void addDestinationMoves(const Position& pos, std::vector<Move>& moves, Bitboard destinations, Move move_base);
   static void addCaptureMoves(const Position& pos, std::vector<Move>& moves, Bitboard captures, Move move_base);

   static void CheckAndSaveMove(const Position& pos, std::vector<Move>& moves, Move proposed_move);
   static bool selfCheckCheck(const Position& pos, Move proposed_move);
   static bool enemyCheckCheck(const Position& pos, Move proposed_move);

   static Bitboard getSlideDestinations(const Position& pos, Bitboard piece, AttackPattern pattern);
   static Bitboard getSlideDestinations(const Position& pos, Bitboard piece, AttackPattern pattern, Bitboard blockers);

   static bool isAttackedBySlidePattern(const Position& pos, Bitboard target, AttackPattern pattern, Bitboard blockers, BoardIndex excluded_index = INVALID_INDEX);
   static bool isAttackedByJumpPattern(const Position& pos, BoardIndex target_index, AttackPattern pattern, BoardIndex excluded_index = INVALID_INDEX);
   // static bool isAttackedByAnyPattern(const Position& pos, Bitboard target, Bitboard blockers); // Moved to public
   static bool canCastleWithRook(const Position& pos, const Bitboard king, const Bitboard rook, const Bitboard king_dest, const Bitboard rook_dest);
};
