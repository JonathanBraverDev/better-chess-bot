#include "Position.h"
#include "Bitboard.h"
#include "Enums.h"
#include "Structs.h"
#include "MoveGenerator.h"
#include "FenUtility.h"
#include "MoveTables.h"
#include <cassert>
#include <sstream>
#include <iostream>
#include <string>

// empty initizlization of static member (now removed)
// PrecomputedMoves Position::precomputed_moves;

void Position::makeMove(Move move) {
  if (move.getMiscMoveType() == MoveType::CASTLE_SHORT ||
      move.getMiscMoveType() == MoveType::CASTLE_LONG) {
    toggleCastle(move);
  } else {
    if (move.isCapture()) {
      toggleCaptured(move);
    }

    if (move.isPromotion()) {
      togglePromotion(move);
    } else {
      toggleMove(move);
    }
  }

  updateSpecialMoveRights(move);
  current_color = getOpponentColor();

  // Invalidate cached data
  legal_moves.clear();
  are_moves_valid = false;
  own_pieces.clear();
  opponent_pieces.clear();
}

Bitboard &Position::getPieceBoardRef(Color color, PieceType type) {
  assert(color != Color::NONE && type != PieceType::NONE);
  // Similar to getPieces but returning a reference
  switch (color) {
  case Color::WHITE:
    switch (type) {
    case PieceType::PAWN:
      return white_pawns;
    case PieceType::KNIGHT:
      return white_knights;
    case PieceType::BISHOP:
      return white_bishops;
    case PieceType::ROOK:
      return white_rooks;
    case PieceType::QUEEN:
      return white_queens;
    case PieceType::KING:
      return white_king;
    default:
      break;
    }
    break;
  case Color::BLACK:
    switch (type) {
    case PieceType::PAWN:
      return black_pawns;
    case PieceType::KNIGHT:
      return black_knights;
    case PieceType::BISHOP:
      return black_bishops;
    case PieceType::ROOK:
      return black_rooks;
    case PieceType::QUEEN:
      return black_queens;
    case PieceType::KING:
      return black_king;
    default:
      break;
    }
    break;
  default:
    break;
  }
  assert(false);
  return white_pawns; // This should NEVER happen
}

void Position::toggleCastle(const Move move) {
  Bitboard &king_board = getPieceBoardRef(current_color, PieceType::KING);
  Bitboard &rook_board = getPieceBoardRef(current_color, PieceType::ROOK);

  bool is_short_castle = (move.getMiscMoveType() == MoveType::CASTLE_SHORT);

  // Use pre-calculated masks to toggle both origin and destination in one go
  if (current_color == Color::WHITE) {
    if (is_short_castle) {
      king_board.toggleBitsFrom(Bitboard(WHITE_SHORT_CASTLE_KING_MASK));
      rook_board.toggleBitsFrom(Bitboard(WHITE_SHORT_CASTLE_ROOK_MASK));
    } else {
      king_board.toggleBitsFrom(Bitboard(WHITE_LONG_CASTLE_KING_MASK));
      rook_board.toggleBitsFrom(Bitboard(WHITE_LONG_CASTLE_ROOK_MASK));
    }
  } else {
    if (is_short_castle) {
      king_board.toggleBitsFrom(Bitboard(BLACK_SHORT_CASTLE_KING_MASK));
      rook_board.toggleBitsFrom(Bitboard(BLACK_SHORT_CASTLE_ROOK_MASK));
    } else {
      king_board.toggleBitsFrom(Bitboard(BLACK_LONG_CASTLE_KING_MASK));
      rook_board.toggleBitsFrom(Bitboard(BLACK_LONG_CASTLE_ROOK_MASK));
    }
  }

  // todo: support Fischer castling where nither pieceis at the usual position
  // in that case use the move's origin and destination to find pieces
}

void Position::toggleMove(const Move move) {
  Bitboard &piece_board =
      getPieceBoardRef(current_color, move.getAbsoluteMovingType());
  piece_board.toggleBit(move.getOriginIndex());
  piece_board.toggleBit(move.getDestinationIndex());
}

void Position::toggleCaptured(const Move move) {
  Bitboard &captured_board =
      getPieceBoardRef(getOpponentColor(), move.getCapturedType());
  uint8_t capture_idx = move.getDestinationIndex();

  // If En Passant, the captured pawn is not on the destination square
  if (move.isEnPassant()) {

    capture_idx = getEnPassantCaptureLocation(current_color, capture_idx)
                      .singleBitIndex();
  }

  captured_board.toggleBit(capture_idx);
}

void Position::togglePromotion(const Move move) {
  // Remove pawn
  getPieceBoardRef(current_color, PieceType::PAWN)
      .toggleBit(move.getOriginIndex());

  // Add promoted piece
  getPieceBoardRef(current_color, move.getAbsoluteMovingType())
      .toggleBit(move.getDestinationIndex());
}

void Position::updateSpecialMoveRights(const Move move) {
  // Clear existing En Passant rights (always valid for one turn only)
  special_move_rights.clearBitsFrom(Bitboard(ALL_EN_PASSANT));

  switch (move.getAbsoluteMovingType()) {
  case PieceType::KING:
    // If the King makes any move, clear all castle rights
    if (current_color == Color::WHITE) {
      special_move_rights.clearBitsFrom(Bitboard(WHITE_CASTLE_ROW));
    } else {
      special_move_rights.clearBitsFrom(Bitboard(BLACK_CASTLE_ROW));
    }
    break;

  case PieceType::ROOK:
    // If a Rook moves, clear its castling right.
    special_move_rights.clearBit(move.getOriginIndex());
    break;

  case PieceType::PAWN:
    // If a pawn jumps, add an En Passant right behind it
    if (move.getMiscMoveType() == MoveType::PAWN_UNIQE && !move.isCapture()) {
      // Mark the destination tile
      Bitboard ep_location(0);
      ep_location.setBit(move.getDestinationIndex());

      // Shift the marked tile to the correct En Passant position
      // White pawns end up above its En Passant tile when jumping
      if (current_color == Color::WHITE) {
        ep_location.shift(Direction::DOWN);
      } else {
        ep_location.shift(Direction::UP);
      }

      special_move_rights.toggleBitsFrom(ep_location);
    }
    break;

  default:
    break;
  }
}

// finds the location of the pawn that was captured by en passant
Bitboard Position::getEnPassantCaptureLocation(Color capturing_color,
                                      BoardIndex en_passant_tile_index) {
  Bitboard capture_location(0);
  capture_location.setBit(en_passant_tile_index);

  // Shift to find the actual pawn location
  // White ends up above the black pawn when capturing
  if (capturing_color == Color::WHITE) {
    capture_location.shift(Direction::DOWN);
  } else {
    capture_location.shift(Direction::UP);
  }

  return capture_location;
}

void Position::CheckAndSaveMove(Move proposed_move) const {
    // Only kept for potential internal usage, or we can just ignore it/remove it
    // since MoveGenerator checks validity.
}

bool Position::selfCheckCheck(Move proposed_move) const {
    // Moved to MoveGenerator
    return false;
}

bool Position::enemyCheckCheck(Move proposed_move) const {
    // Moved to MoveGenerator
    return false;
}

// These helper check functions are now in MoveGenerator, or we need to update Position if we kept them.
// But we moved them. So we remove valid implementation here.

Color Position::getOpponentColor() const {
  return (current_color == Color::WHITE ? Color::BLACK : Color::WHITE);
}

Bitboard Position::getOpponentEnPassant() const {
  return Bitboard(
      (current_color == Color::WHITE ? BLACK_EN_PASSANT : WHITE_EN_PASSANT));
}

Bitboard Position::getPieces(Color color, PieceType type) const {
  assert(color != Color::NONE && type != PieceType::NONE);
  switch (color) {
  case Color::WHITE:
    switch (type) {
    case PieceType::PAWN:
      return white_pawns;
    case PieceType::KNIGHT:
      return white_knights;
    case PieceType::BISHOP:
      return white_bishops;
    case PieceType::ROOK:
      return white_rooks;
    case PieceType::QUEEN:
      return white_queens;
    case PieceType::KING:
      return white_king;
    }
    break;
  case Color::BLACK:
    switch (type) {
    case PieceType::PAWN:
      return black_pawns;
    case PieceType::KNIGHT:
      return black_knights;
    case PieceType::BISHOP:
      return black_bishops;
    case PieceType::ROOK:
      return black_rooks;
    case PieceType::QUEEN:
      return black_queens;
    case PieceType::KING:
      return black_king;
    }
    break;
  }
  assert(false);
  return Bitboard(0);
}

Bitboard Position::getPiecesByPattern(Color color,
                                      AttackPattern pattern) const {
  switch (pattern) {
  case AttackPattern::PAWN:
    return getPieces(color, PieceType::PAWN);
  case AttackPattern::KNIGHT:
    return getPieces(color, PieceType::KNIGHT);
  case AttackPattern::KING:
    return getPieces(color, PieceType::KING);
  case AttackPattern::DIAGONAL:
    return combineBoards(getPieces(color, PieceType::BISHOP),
                                   getPieces(color, PieceType::QUEEN));
  case AttackPattern::LINE:
    return combineBoards(getPieces(color, PieceType::ROOK),
                                   getPieces(color, PieceType::QUEEN));
  }
    return Bitboard(0);
}

Piece Position::getPieceAtIndex(BoardIndex index) const {
  Bitboard tile = (1ULL << index);
  return getPieceAtTile(tile);
}

Piece Position::getPieceAtTile(Bitboard tile) const {
  if (findCommonBits(white_pawns, tile).hasRemainingBits()) {
    return {Color::WHITE, PieceType::PAWN};
  } else if (findCommonBits(black_pawns, tile).hasRemainingBits()) {
    return {Color::BLACK, PieceType::PAWN};
  } else if (findCommonBits(white_knights, tile).hasRemainingBits()) {
    return {Color::WHITE, PieceType::KNIGHT};
  } else if (findCommonBits(black_knights, tile).hasRemainingBits()) {
    return {Color::BLACK, PieceType::KNIGHT};
  } else if (findCommonBits(white_bishops, tile).hasRemainingBits()) {
    return {Color::WHITE, PieceType::BISHOP};
  } else if (findCommonBits(black_bishops, tile).hasRemainingBits()) {
    return {Color::BLACK, PieceType::BISHOP};
  } else if (findCommonBits(white_rooks, tile).hasRemainingBits()) {
    return {Color::WHITE, PieceType::ROOK};
  } else if (findCommonBits(black_rooks, tile).hasRemainingBits()) {
    return {Color::BLACK, PieceType::ROOK};
  } else if (findCommonBits(white_queens, tile).hasRemainingBits()) {
    return {Color::WHITE, PieceType::QUEEN};
  } else if (findCommonBits(black_queens, tile).hasRemainingBits()) {
    return {Color::BLACK, PieceType::QUEEN};
  } else if (findCommonBits(white_king, tile).hasRemainingBits()) {
    return {Color::WHITE, PieceType::KING};
  } else if (findCommonBits(black_king, tile).hasRemainingBits()) {
    return {Color::BLACK, PieceType::KING};
  } else {
    return {Color::NONE, PieceType::NONE};
  }
}

std::vector<Move> Position::getLegalMoves() const {
  if (!are_moves_valid) {
    getAllOwnPieces();
    getAllOpponentPieces();

    // Use MoveGenerator
    legal_moves = MoveGenerator::getLegalMoves(*this);

    are_moves_valid = true;
  }

  return legal_moves;
}

Bitboard Position::getOwnPieces(PieceType type) const {
  return getPieces(current_color, type);
}

Bitboard Position::getOpponentPieces(PieceType type) const {
  return getPieces(getOpponentColor(), type);
}

Bitboard Position::getAllOwnPieces() const {
  if (own_pieces.isEmpty()) {
    own_pieces = combineBoards(
        getOwnPieces(PieceType::PAWN), getOwnPieces(PieceType::KNIGHT),
        getOwnPieces(PieceType::BISHOP), getOwnPieces(PieceType::ROOK),
        getOwnPieces(PieceType::QUEEN), getOwnPieces(PieceType::KING));
  }
  return own_pieces;
}

Bitboard Position::getAllOpponentPieces() const {
  if (opponent_pieces.isEmpty()) {
    opponent_pieces = combineBoards(
        getOpponentPieces(PieceType::PAWN),
        getOpponentPieces(PieceType::KNIGHT),
        getOpponentPieces(PieceType::BISHOP),
        getOpponentPieces(PieceType::ROOK), getOpponentPieces(PieceType::QUEEN),
        getOpponentPieces(PieceType::KING));
  }
  return opponent_pieces;
}

Bitboard Position::getAllPieces() const {
  return combineBoards(getAllOwnPieces(), getAllOpponentPieces());
}

void Position::InitializeMoves() {
    MoveTables::initialize();
}

bool Position::isInCheck() const {
    Bitboard king = getPieces(current_color, PieceType::KING);
    return MoveGenerator::isAttackedByAnyPattern(*this, king, getAllPieces());
}

Move Position::currentBitRights() const {
  Move rights(0);

  // operating directly regardless of color
  if (findCommonBits(white_king, special_move_rights)
          .hasRemainingBits()) {
    rights.setWhiteLongCastleRight(
        findCommonBits(white_king.lowerThanSingleBit(), white_rooks,
                                 special_move_rights)
            .hasRemainingBits());

    rights.setWhiteShortCastleRight(
        findCommonBits(white_king.higherThanSingleBit(), white_rooks,
                                 special_move_rights)
            .hasRemainingBits());
  }

  if (findCommonBits(black_king, special_move_rights)
          .hasRemainingBits()) {
    rights.setBlackLongCastleRight(
        findCommonBits(black_king.lowerThanSingleBit(), black_rooks,
                                 special_move_rights)
            .hasRemainingBits());

    rights.setBlackShortCastleRight(
        findCommonBits(black_king.higherThanSingleBit(), black_rooks,
                                 special_move_rights)
            .hasRemainingBits());
  }

  if (findCommonBits(getOpponentEnPassant(), special_move_rights)
          .hasRemainingBits()) {
    rights.setEnPassantIndex(
        findCommonBits(Bitboard(ALL_EN_PASSANT), special_move_rights)
            .singleBitIndex() %
        8);
  }

  return rights;
}

Position::Position() {
  white_pawns.clear();
  white_knights.clear();
  white_bishops.clear();
  white_rooks.clear();
  white_queens.clear();
  white_king.clear();
  black_pawns.clear();
  black_knights.clear();
  black_bishops.clear();
  black_rooks.clear();
  black_queens.clear();
  black_king.clear();
  special_move_rights.clear();
  own_pieces.clear();
  opponent_pieces.clear();
  legal_moves.clear();
  are_moves_valid = false;
  current_color = Color::WHITE;
}

Position Position::fromFen(FenString fen) {
  return FenUtility::fromFen(fen);
}

std::string Position::toFen() const {
  return FenUtility::toFen(*this);
}
