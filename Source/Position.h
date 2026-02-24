#pragma once

#include "Bitboard.h"
#include "Move.h"
#include "Structs.h"
#include "MoveList.h"
#include <vector>
#include <string>
#include <cassert>

// out of structs.h cuse codependancy mess
// all the needed information to make a legal move
class Position {
  friend class FenUtility;
  friend class MoveGenerator;

private:
  // Array bitboard storage: [Color][PieceType]
  // keys by the respective Enums
  Bitboard pieces[2][6];

  inline int colIdx(Color c) const {
      return static_cast<int>(c);
  }
  inline int typeIdx(PieceType t) const {
      assert(t != PieceType::NONE); // prevent negative index access
      return static_cast<int>(t) -1;
  }

  // en passant AND castle rights for both sides, they can't overlap anyway
  // used to calculate BitRights for all moves from the position
  Bitboard special_move_rights;
  Color current_color;

  // greatly reducing cluttered calls during move generation
  // remember to wipe when ANYTHING changes on the board
  mutable Bitboard own_pieces;
  mutable Bitboard opponent_pieces;

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
  void undoMove(Move move);

  Bitboard getPieces(Color color, PieceType type) const;
  Piece getPieceAtIndex(BoardIndex index) const;
  Piece getPieceAtTile(Bitboard tile) const;

  MoveList getLegalMoves() const;

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
  Bitboard getOpponentEnPassantRow() const;
  
  // Helpers for MoveGenerator
  static Bitboard getEnPassantCaptureLocation(Color capturing_color, BoardIndex en_passant_tile_index);
};
