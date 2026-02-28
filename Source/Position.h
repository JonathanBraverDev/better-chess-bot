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

  // incrementally updated on make/undo move to reduce cluttered calls
  Bitboard color_pieces[2];

  // Helper to re-initialize caches when entire position is overwritten (e.g. from FEN)
  // Must be called after directly writing to piece boards
  void updateCachedPieces();

  // Helpers for make/undo move
  Bitboard &getPieceBoardRef(Color color, PieceType type);
  inline void togglePiece(Color color, PieceType type, BoardIndex index) {
      getPieceBoardRef(color, type).toggleBit(index);
      color_pieces[colIdx(color)].toggleBit(index);
  }
  
  void toggleCastle(const Move move);
  void toggleMove(const Move move);
  void toggleCaptured(const Move move);
  void togglePromotion(const Move move);
  void updateSpecialMoveRights(const Move move);
  void restoreSpecialMoveRights(const Move move);



public:
  // allow initialization from string
  Position();
  static Position fromFen(FenString fen);
  std::string toFen() const;

  void makeMove(Move move);
  void undoMove(Move move);

  Bitboard getPieces(Color color, PieceType type) const;
  Bitboard getPieces(PieceType type) const;
  Piece getPieceAtIndex(BoardIndex index) const;
  Piece getPieceAtTile(Bitboard tile) const;

  MoveList getLegalMoves() const;

  Bitboard getOwnPieces(PieceType type) const;
  Bitboard getOpponentPieces(PieceType type) const;
  Bitboard getPiecesByPattern(Color color, AttackPattern pattern) const;
  Bitboard getPiecesByPattern(AttackPattern pattern) const;
  Bitboard getOpponentPiecesByPattern(AttackPattern pattern) const;
  Bitboard getAllOwnPieces() const;
  Bitboard getAllOpponentPieces() const;
  Bitboard getAllPieces() const;

  static void InitializeMoves();

  // Creates an empty move with BitRights from the special move board
  Move currentBitRights() const;
  
  // Creates an initialized move containing current BitRights and the given PieceType
  Move initializeMove(PieceType type) const;

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
