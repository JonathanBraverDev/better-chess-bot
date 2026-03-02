#pragma once

#include "MoveList.h"
#include "Position.h"

class MoveGenerator {
public:
  static MoveList getLegalMoves(const Position &pos);
  static bool isAttackedByAnyPattern(const Position &pos, Bitboard target,
                                     Bitboard blockers);

private:
  const Position &pos;
  MoveList moves;

  // Cached position data for this generation pass
  Color current_color;
  Bitboard empty_tiles;
  Bitboard opponent_en_passant;
  Bitboard all_own_pieces;
  Bitboard all_opponent_pieces;
  Bitboard all_pieces;

  MoveGenerator(const Position &p);
  MoveList generateLegalMoves();

  void getPawnMoves();
  void getKnightMoves();
  void getBishopMoves();
  void getRookMoves();
  void getQueenMoves();
  void getKingMoves();

  void getSlidingPieceMoves(PieceType pieceType);
  void getCastlingMoves(Bitboard king, Bitboard blockers, Move move_base);

  // Helpers
  void checkAndAddPawnJump(Bitboard step, Move move_base, Direction forward);
  void checkAndAddEnPassant(Bitboard possible_en_passant, int pawn_move_index,
                            Move move_base);
  void addPromotionMoves(Bitboard step, Bitboard captures, Move move_base);

  void finalizeMoves(Bitboard destinations, Move move_base);

  void CheckAndSaveMove(Move proposed_move);
  bool selfCheckCheck(Move proposed_move) const;
  bool enemyCheckCheck(Move proposed_move) const;

  Bitboard getSlideDestinations(Bitboard piece, AttackPattern pattern) const;
  Bitboard getSlideDestinations(Bitboard piece, AttackPattern pattern,
                                Bitboard blockers) const;

  bool
  isAttackedBySlidePattern(Bitboard target, AttackPattern pattern,
                           Bitboard blockers,
                           BoardIndex excluded_index = INVALID_INDEX) const;
  bool isAttackedByJumpPattern(BoardIndex target_index, AttackPattern pattern,
                               BoardIndex excluded_index = INVALID_INDEX) const;
  bool canCastleWithRook(const Bitboard king, const Bitboard rook,
                         const Bitboard king_dest,
                         const Bitboard rook_dest) const;
};
