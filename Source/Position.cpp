#include "Position.h"
#include "Bitboard.h"
#include "Enums.h"
#include "Structs.h"
#include "MoveGenerator.h"
#include "FenUtility.h"
#include "MoveTables.h"
#include "BoardConstants.h"
#include <cassert>
#include <string>


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
}

void Position::restoreSpecialMoveRights(const Move move) {
  special_move_rights.clear();
  
  if (move.getWhiteShortCastleRight() || move.getWhiteLongCastleRight()) {
      special_move_rights.setBit(E1_index);
  }
  if (move.getWhiteShortCastleRight()) special_move_rights.setBit(H1_index);
  if (move.getWhiteLongCastleRight()) special_move_rights.setBit(A1_index);

  if (move.getBlackShortCastleRight() || move.getBlackLongCastleRight()) {
      special_move_rights.setBit(E8_index);
  }
  if (move.getBlackShortCastleRight()) special_move_rights.setBit(H8_index);
  if (move.getBlackLongCastleRight()) special_move_rights.setBit(A8_index);
  
  if (move.isValidEnPassant()) {
    BoardIndex ep_idx = move.getEnPassantIndex();
    special_move_rights.setBitsFrom(findCommonBits(getOpponentEnPassantRow(), Bitboard(COLUMN_A << ep_idx)));
  }
}

void Position::undoMove(Move move) {
  current_color = getOpponentColor(); // Change back to the color that made the move
  
  // Restore special move rights directly from the Move's bit rights
  restoreSpecialMoveRights(move);

  if (move.getMiscMoveType() == MoveType::CASTLE_SHORT ||
      move.getMiscMoveType() == MoveType::CASTLE_LONG) {
    toggleCastle(move); // toggling castle again reverses it
  } else {
    if (move.isPromotion()) {
      togglePromotion(move); // toggling promotion again reverses it
    } else {
      toggleMove(move); // toggling move again reverses it
    }

    if (move.isCapture()) {
      toggleCaptured(move); // toggling capture again reverses it
    }
  }
}

Bitboard &Position::getPieceBoardRef(Color color, PieceType type) {
  assert(type != PieceType::NONE);
  return pieces[colIdx(color)][typeIdx(type)];
}

void Position::toggleCastle(const Move move) {
  Bitboard &king_board = getPieceBoardRef(current_color, PieceType::KING);
  Bitboard &rook_board = getPieceBoardRef(current_color, PieceType::ROOK);

  int colorIdx = colIdx(current_color);
  bool is_short_castle = (move.getMiscMoveType() == MoveType::CASTLE_SHORT);

  Bitboard king_mask = Bitboard(CASTLE_MASKS[colorIdx][is_short_castle].king);
  Bitboard rook_mask = Bitboard(CASTLE_MASKS[colorIdx][is_short_castle].rook);

  king_board.toggleBitsFrom(king_mask);
  rook_board.toggleBitsFrom(rook_mask);
  color_pieces[colorIdx].toggleBitsFrom(king_mask);
  color_pieces[colorIdx].toggleBitsFrom(rook_mask);

  // todo: support Fischer castling where nither pieceis at the usual position
  // in that case use the move's origin and destination to find pieces
}

void Position::toggleMove(const Move move) {
  PieceType moving_type = move.getAbsoluteMovingType();
  togglePiece(current_color, moving_type, move.getOriginIndex());
  togglePiece(current_color, moving_type, move.getDestinationIndex());
}

void Position::toggleCaptured(const Move move) {
  BoardIndex capture_idx = move.getDestinationIndex();

  // If En Passant, the captured pawn is not on the destination square
  if (move.isEnPassant()) {
    capture_idx = getEnPassantCaptureLocation(current_color, capture_idx)
                      .singleBitIndex();
  }

  togglePiece(getOpponentColor(), move.getCapturedType(), capture_idx);
}

void Position::togglePromotion(const Move move) {
  // Remove pawn
  togglePiece(current_color, PieceType::PAWN, move.getOriginIndex());

  // Add promoted piece
  togglePiece(current_color, move.getAbsoluteMovingType(), move.getDestinationIndex());
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

Bitboard Position::getOpponentEnPassantRow() const {
  return Bitboard(
      (current_color == Color::WHITE ? BLACK_EN_PASSANT : WHITE_EN_PASSANT));
}

Bitboard Position::getPieces(Color color, PieceType type) const {
  assert(type != PieceType::NONE);
  return pieces[colIdx(color)][typeIdx(type)];
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
  return getPieceAtTile(Bitboard::boardFromIndex(index));
}

Piece Position::getPieceAtTile(Bitboard tile) const {
    if (findCommonBits(color_pieces[0], tile).hasRemainingBits()) {
        for (PieceType t : PieceTypes) {
            if (findCommonBits(pieces[0][typeIdx(t)], tile).hasRemainingBits()) {
                return {Color::WHITE, t};
            }
        }
    } else if (findCommonBits(color_pieces[1], tile).hasRemainingBits()) {
        for (PieceType t : PieceTypes) {
            if (findCommonBits(pieces[1][typeIdx(t)], tile).hasRemainingBits()) {
                return {Color::BLACK, t};
            }
        }
    }
    return {Color::WHITE, PieceType::NONE};
}

MoveList Position::getLegalMoves() const {
  return MoveGenerator::getLegalMoves(*this);
}

Bitboard Position::getPieces(PieceType type) const {
  return getPieces(current_color, type);
}

Bitboard Position::getPiecesByPattern(AttackPattern pattern) const {
  return getPiecesByPattern(current_color, pattern);
}

Bitboard Position::getOpponentPiecesByPattern(AttackPattern pattern) const {
  return getPiecesByPattern(getOpponentColor(), pattern);
}

Bitboard Position::getOwnPieces(PieceType type) const {
  return getPieces(current_color, type);
}

Bitboard Position::getOpponentPieces(PieceType type) const {
  return getPieces(getOpponentColor(), type);
}

Bitboard Position::getAllOwnPieces() const {
  return color_pieces[colIdx(current_color)];
}

Bitboard Position::getAllOpponentPieces() const {
  return color_pieces[colIdx(getOpponentColor())];
}

Bitboard Position::getAllPieces() const {
  return combineBoards(color_pieces[0], color_pieces[1]);
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

  Bitboard white_king = pieces[colIdx(Color::WHITE)][typeIdx(PieceType::KING)];
  Bitboard black_king = pieces[colIdx(Color::BLACK)][typeIdx(PieceType::KING)];
  Bitboard white_rooks = pieces[colIdx(Color::WHITE)][typeIdx(PieceType::ROOK)];
  Bitboard black_rooks = pieces[colIdx(Color::BLACK)][typeIdx(PieceType::ROOK)];

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

  if (findCommonBits(getOpponentEnPassantRow(), special_move_rights)
          .hasRemainingBits()) {
    rights.setEnPassantIndex(
        findCommonBits(Bitboard(ALL_EN_PASSANT), special_move_rights)
            .singleBitIndex() %
        8);
  }

  return rights;
}

Move Position::initializeMove(PieceType type) const {
    Move move = currentBitRights();
    move.setMovingType(type);
    return move;
}

Position::Position() {
  for (Color c : Colors) {
      for (PieceType t : PieceTypes) {
          pieces[colIdx(c)][typeIdx(t)].clear();
      }
  }

  special_move_rights.clear();
  color_pieces[0].clear();
  color_pieces[1].clear();
  current_color = Color::WHITE;
}

void Position::updateCachedPieces() {
  color_pieces[0].clear();
  color_pieces[1].clear();
  for (PieceType t : PieceTypes) {
    color_pieces[0].setBitsFrom(pieces[colIdx(Color::WHITE)][typeIdx(t)]);
    color_pieces[1].setBitsFrom(pieces[colIdx(Color::BLACK)][typeIdx(t)]);
  }
}

Position Position::fromFen(FenString fen) {
  return FenUtility::fromFen(fen);
}

std::string Position::toFen() const {
  return FenUtility::toFen(*this);
}

