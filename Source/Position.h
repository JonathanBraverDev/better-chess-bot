#pragma once

#include "Bitboard.h"
#include "Move.h"
#include "Structs.h"
#include <vector>
#include <string>

// out of structs.h cuse codependancy mess
struct PrecomputedMoves {
  Bitboard king_moves[64];
  Bitboard knight_moves[64];
  Bitboard pawn_moves[64 * 2];
  Bitboard pawn_attacks[64 * 2];
};

// all the needed information to make a legal move
class Position {
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

  // en passant AND castle rights for both sides, they can't overlap anyway
  // used to calculate BitRights for all moves from the position
  Bitboard special_move_rights;
  Color current_color;

  // greatly reducing cluttered calls during move generation
  // remember to wipe when ANYTHING changes on the board
  mutable Bitboard own_pieces;
  mutable Bitboard opponent_pieces;
  // moves that get here should be fully legal.
  mutable std::vector<Move> legal_moves;
  mutable bool are_moves_valid;

  static PrecomputedMoves precomputed_moves;

  Color getOpponentColor() const;
  Bitboard getOpponentEnPassant() const;

  // moves the pieces could make
  void getPawnMoves() const;
  void checkAndAddPawnJump(Bitboard step, Bitboard empty_tiles, Move move_base,
                           Direction forward) const;
  void checkAndAddEnPassant(Bitboard possible_en_passant, int pawn_move_index,
                            Move move_base) const;
  void addPromotionMoves(Bitboard step, Bitboard captures,
                         Move move_base) const;
  void addNormalPawnMoves(Move base_move, Bitboard step,
                          Bitboard captures) const;
  void getKnightMoves() const;
  void getKingMoves() const;
  void getCastlingMoves(Bitboard king, Bitboard blockers, Move move_base) const;
  bool canCastleWithRook(const Bitboard king, const Bitboard rook,
                 const Bitboard king_dest, const Bitboard rook_dest) const;
  inline void getBishopMoves() const;
  inline void getRookMoves() const;
  inline void getQueenMoves() const;
  void getSlidingPieceMoves(const PieceType pieceType) const;
  Bitboard getSlideDestinations(const Bitboard piece,
                                const AttackPattern pattern) const;
  Bitboard getSlideDestinations(const Bitboard piece,
                                const AttackPattern pattern,
                                const Bitboard blockers) const;

  void finalizeMoves(Bitboard destinations, Move move_base) const;
  void addDestinationMoves(Bitboard destinations, Move move_base) const;
  void addCaptureMoves(Bitboard captures, Move move_base) const;

  void CheckAndSaveMove(Move proposed_move) const;
  bool selfCheckCheck(Move proposed_move) const;
  bool isAttackedBySlidePattern(Bitboard target, AttackPattern pattern,
                                Bitboard blockers) const;

  bool isAttackedByJumpPattern(BoardIndex target_index,
                               AttackPattern pattern,
                               BoardIndex excluded_index = INVALID_INDEX) const;
  bool isAttackedByAnyPattern(Bitboard target, Bitboard blockers) const;
  bool enemyCheckCheck(Move proposed_move) const;

  static void PrepareKingMoves();
  static void PrepareKnightMoves();
  static void PrepareWhitePawnMoves();
  static void PrepareBlackPawnMoves();

  static Bitboard getEnPassantCaptureLocation(Color capturing_color,
                                              BoardIndex en_passant_tile_index);

  // Helpers for make/undo move
  Bitboard &getPieceBoardRef(Color color, PieceType type);
  void toggleCastle(const Move move);
  void toggleMove(const Move move);
  void toggleCaptured(const Move move);
  void togglePromotion(const Move move);
  void updateSpecialMoveRights(const Move move);

public:
  // allow initialization from string
  Position();
  static Position fromFen(FenString fen);
  std::string toFen() const;

  // TODO: Add a constructor that creates a position from moves

  void makeMove(Move move);

  Bitboard getPieces(Color color, PieceType type) const;
  Piece getPieceAtIndex(BoardIndex index) const;
  Piece getPieceAtTile(Bitboard tile) const;

  std::vector<Move> getLegalMoves() const;

  Bitboard getOwnPieces(PieceType type) const;
  Bitboard getOpponentPieces(PieceType type) const;
  Bitboard getPiecesByPattern(Color color, AttackPattern pattern) const;
  Bitboard getAllOwnPieces() const;
  Bitboard getAllOpponentPieces() const;
  Bitboard getAllPieces() const;

  static void InitializeMoves();

  // Creates an empty move with BitRights from the special move board
  Move currentBitRights() const;

  bool isInCheck() const;
};
