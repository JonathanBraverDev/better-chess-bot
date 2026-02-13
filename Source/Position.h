#pragma once

#include "Bitboard.h"
#include "Move.h"
#include "Structs.h"
#include <vector>
#include <string>

// out of structs.h cuse codependancy mess
// all the needed information to make a legal move
class Position {
  friend class FenUtility;
  friend class MoveGenerator;

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

  // Helpers for make/undo move
  Bitboard &getPieceBoardRef(Color color, PieceType type);
  void toggleCastle(const Move move);
  void toggleMove(const Move move);
  void toggleCaptured(const Move move);
  void togglePromotion(const Move move);
  void updateSpecialMoveRights(const Move move);

  // Helper filters for MoveGenerator (formerly private)
  bool selfCheckCheck(Move proposed_move) const;
  bool enemyCheckCheck(Move proposed_move) const;
  void CheckAndSaveMove(Move proposed_move) const;

public:
  // allow initialization from string
  Position();
  static Position fromFen(FenString fen);
  std::string toFen() const;

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

    // Accessors needed by MoveGenerator and FenUtility
  Color getCurrentColor() const { return current_color; }
  void setCurrentColor(Color color) { current_color = color; }
  Bitboard getSpecialMoveRights() const { return special_move_rights; }
  Bitboard& getSpecialMoveRightsRef() { return special_move_rights; }
  Color getOpponentColor() const;
  Bitboard getOpponentEnPassant() const;
  
  // Helpers for MoveGenerator
  static Bitboard getEnPassantCaptureLocation(Color capturing_color, BoardIndex en_passant_tile_index);
};
